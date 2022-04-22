//
//  kern_rtls.cpp
//  Rootless
//
//  Created by flagers on 4/14/22.
//  Copyright © 2022 flagers. All rights reserved.
//

// Lilu headers
#include <Headers/kern_api.hpp>
// Project headers
#include "kern_rtls.hpp"

static RTLS rootless;

RTLS* RTLS::callbackRtls = nullptr;

void RTLS::createShared() {
    if (callbackRtls)
        PANIC(MODULE_SHORT, "Attempted to assign Rootless callback again");
    callbackRtls = &rootless;
    if (!callbackRtls)
        PANIC(MODULE_SHORT, "Failed to assign Rootless callback");
}

void RTLS::init() {
    lilu.onPatcherLoadForce([](void *user, KernelPatcher &patcher) {
        static_cast<RTLS *>(user)->codesign.init(patcher);
    }, this);
}

void RTLS::deinit() {

}

void RTLS::Codesign::CDHash::init(KernelPatcher &patcher) {
    if (callbackCdhs)
        PANIC(MODULE_SHORT, "Attempted to assign Rootless CDHash callback again");
    callbackCdhs = this;
    if (!callbackCdhs)
        PANIC(MODULE_SHORT, "Failed to assign Rootless CDHash callback");
    
    /* Platform Binary Override Initialization */
    platformCDHashes = OSArray::withCapacity(1);
#ifdef DEBUG
    IORegistryEntry *entry;
    OSArray *cdhashArray;
    if (NULL != (entry = IORegistryEntry::fromPath("/IOResources/Rootless", gIOServicePlane)))
        if (NULL != (cdhashArray = (OSArray *)entry->getProperty("Platform CDHashes")))
            platformCDHashes->merge(cdhashArray);
#endif

    orgCsfgGetTeamId = reinterpret_cast<decltype(orgCsfgGetTeamId)>(patcher.solveSymbol(KernelPatcher::KernelID, "_csfg_get_teamid"));
    orgCsfgGetPath = reinterpret_cast<decltype(orgCsfgGetPath)>(patcher.solveSymbol(KernelPatcher::KernelID, "_csfg_get_path"));
    orgCsfgGetCdhash = reinterpret_cast<decltype(orgCsfgGetCdhash)>(patcher.solveSymbol(KernelPatcher::KernelID, "_csfg_get_cdhash"));
}

int RTLS::Codesign::CDHash::wrapCsfgGetPlatformBinary(struct fileglob *fg) {
    int result = FunctionCast(wrapCsfgGetPlatformBinary, callbackCdhs->orgCsfgGetPlatformBinary)(fg);
    if (!result) {
        const char *teamId = callbackCdhs->orgCsfgGetTeamId(fg);
        if (teamId && !strcmp(teamId, mykolaTeamId)) {
            DBGLOG("cdhs", "platform binary override for Team ID (%s)", mykolaTeamId);
            result = 1;
        } else {
            if (callbackCdhs->verifyFromFileglob(fg)) { result = 1; }
        }
    }

    return result;
}

bool RTLS::Codesign::CDHash::verifyFromFileglob(struct fileglob *fg) {
    bool result = false;
    auto machHeader = Buffer::create<uint8_t>(sizeof(mach_header_64));
    if (!machHeader) {
        SYSLOG("cdhs", "failed to allocate space for mach header");
    } else {
        // I hate getting the file path, then looking up the vnode for that path.
        // There must be a better solution for this, but I don't feel comfortable
        // mucking around with using fileglob offsets that are prone to change.
        int len = PATH_MAX;
        char *path = (char *)Buffer::create<uint8_t>(PATH_MAX);
        if (path) {
            if (orgCsfgGetPath(fg, path, &len)) {
                SYSLOG("cdhs", "failed to get file path");
            } else {
                DBGLOG("cdhs", "verifyFromFileglob - path: %s", path);
                vnode_t vnode = NULLVP;
                vfs_context_t ctxt = vfs_context_create(nullptr);
                
                errno_t err = vnode_lookup(path, 0, &vnode, ctxt);
                if (!err) {
                    int error = FileIO::readFileData(machHeader, 0, sizeof(mach_header_64), vnode, ctxt);
                    if (error) {
                        SYSLOG("cdhs", "failed to read mach header with error: %d", error);
                    } else {
                        auto magicPtr = reinterpret_cast<uint32_t *>(machHeader);
                        if (!isAligned(magicPtr)) {
                            SYSLOG("cdhs", "invalid mach header positioning");
                        } else {
                            UInt64 slice_offset = 0;
                            size_t cdhash_size = 0;
                            switch (*magicPtr) {
                                case MH_MAGIC_64:
                                    result = cdhashAllowed(orgCsfgGetCdhash(fg, slice_offset, &cdhash_size));
                                case FAT_CIGAM:
                                case FAT_MAGIC:
                                    // TODO: handle FAT binaries in here
                                    // If the CDHash of ANY slice of the FAT binary
                                    // is not in the allowed CDHash array, then do
                                    // not override as a platform binary.
                                    char *name = strrchr(path, '/')+1;
                                    DBGLOG("cdhs", "verifyFromFileglob - will not except FAT binary (%s)", name ? name : "unknown");
                            }
                        }
                    }
                }
                vfs_context_rele(ctxt);
            }
            Buffer::deleter(path);
        } else {
            SYSLOG("cdhs", "failed to allocate space for path");
        }
        Buffer::deleter(machHeader);
    }
    return result;
}

bool RTLS::Codesign::CDHash::cdhashAllowed(uint8_t *cdhashToCheck) {
    OSCollectionIterator *iterator;
    OSData *cdhash;
    bool allowed = false;
    
    if (NULL != (iterator = OSCollectionIterator::withCollection(platformCDHashes))) {
        while (NULL != (cdhash = (OSData *)iterator->getNextObject())) {
            if (!allowed) {
                if (cdhash->isEqualTo(cdhashToCheck, 20)) {
                    allowed = true;
                    DBGLOG("cdhs", "CDHash is in override database");
                } else {
                    DBGLOG("cdhs", "CDHash is not in override database");
                }
            }
        }
    }
    iterator->release();
    return allowed;
}
