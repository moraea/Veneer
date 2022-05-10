//
//  cdhash.cpp
//  Rootless
//
//  Created by flagers on 5/10/22.
//  Copyright © 2022 flagers. All rights reserved.
//

// Lilu headers
#include <Headers/kern_patcher.hpp>
#include <Headers/kern_file.hpp>
// Project headers
#include "kern_rtls.hpp"

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

    orgGetBsdTaskInfo = reinterpret_cast<decltype(orgGetBsdTaskInfo)>(patcher.solveSymbol(KernelPatcher::KernelID, "_get_bsdtask_info"));
    orgCsGetCdhash = reinterpret_cast<decltype(orgCsGetCdhash)>(patcher.solveSymbol(KernelPatcher::KernelID, "_cs_get_cdhash"));
    orgCsprocGetTeamId = reinterpret_cast<decltype(orgCsprocGetTeamId)>(patcher.solveSymbol(KernelPatcher::KernelID, "_csproc_get_teamid"));
    orgCsfgGetTeamId = reinterpret_cast<decltype(orgCsfgGetTeamId)>(patcher.solveSymbol(KernelPatcher::KernelID, "_csfg_get_teamid"));
    orgCsfgGetPath = reinterpret_cast<decltype(orgCsfgGetPath)>(patcher.solveSymbol(KernelPatcher::KernelID, "_csfg_get_path"));
    orgCsfgGetCdhash = reinterpret_cast<decltype(orgCsfgGetCdhash)>(patcher.solveSymbol(KernelPatcher::KernelID, "_csfg_get_cdhash"));
    orgCsblobGetTeamId = reinterpret_cast<decltype(orgCsblobGetTeamId)>(patcher.solveSymbol(KernelPatcher::KernelID, "_csblob_get_teamid"));
    orgCsblobGetCdhash = reinterpret_cast<decltype(orgCsblobGetCdhash)>(patcher.solveSymbol(KernelPatcher::KernelID, "_csblob_get_cdhash"));
    
    if (!(orgGetBsdTaskInfo &&
              orgCsGetCdhash &&
              orgCsprocGetTeamId &&
              orgCsfgGetTeamId &&
              orgCsfgGetPath &&
              orgCsfgGetCdhash)) {
            PANIC("cdhs", "failed to solve required symbols for RTLS Codesign");
    }
    
    KernelPatcher::RouteRequest requests[] {
        KernelPatcher::RouteRequest("_csfg_get_platform_binary", wrapCsfgGetPlatformBinary, orgCsfgGetPlatformBinary),
        KernelPatcher::RouteRequest("_csblob_get_platform_binary", wrapCsblobGetPlatformBinary, orgCsblobGetPlatformBinary),
        KernelPatcher::RouteRequest("_csproc_get_platform_binary", wrapCsprocGetPlatformBinary, orgCsprocGetPlatformBinary)
    };
    if (!patcher.routeMultipleLong(KernelPatcher::KernelID, requests, arrsize(requests)))
        PANIC("cdhs", "failed to route functions for RTLS Codesign");
    
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

/* static */
int RTLS::Codesign::CDHash::wrapCsblobGetPlatformBinary(struct cs_blob *blob) {
    int result = FunctionCast(wrapCsblobGetPlatformBinary, callbackCdhs->orgCsblobGetPlatformBinary)(blob);
    if (!result) {
        const char *teamId = callbackCdhs->orgCsblobGetTeamId(blob);
        if (teamId && !strcmp(teamId, mykolaTeamId)) {
            DBGLOG("cdhs", "platform binary override for %s", mykolaTeamId);
            result = 1;
        } else {
            if (callbackCdhs->cdhashAllowed(callbackCdhs->orgCsblobGetCdhash(blob)))
                result = 1;
        }
    }
    
    return result;
}

/* static */
int RTLS::Codesign::CDHash::wrapCsprocGetPlatformBinary(struct proc *p) {
    int result = FunctionCast(wrapCsprocGetPlatformBinary, callbackCdhs->orgCsprocGetPlatformBinary)(p);
    if (!result) {
        const char *teamId = callbackCdhs->orgCsprocGetTeamId(p);
        if (teamId && !strcmp(teamId, mykolaTeamId)) {
            DBGLOG("cdhs", "platform binary override for %s", mykolaTeamId);
            result = 1;
        } else {
            if (callbackCdhs->cdhashAllowed(callbackCdhs->orgCsGetCdhash(p)))
                result = 1;
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
        while (NULL != (cdhash = OSDynamicCast(OSData, iterator->getNextObject()))) {
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

IOReturn RTLS::Codesign::CDHash::addCdhashAction(OSDictionary *action) {
    OSString *actionName = OSDynamicCast(OSString, action->getObject("Action"));
    if (!actionName)
        return kIOReturnInvalid;
    
    if (!actionName->isEqualTo("Make CDHash a Platform Binary"))
        return kIOReturnInvalid;
    
    OSData *cdhashToAdd = OSDynamicCast(OSData, action->getObject("CDHash"));
    if (!cdhashToAdd)
        return kIOReturnInvalid;
    
    if (!platformCDHashes->setObject(OSData::withData(cdhashToAdd)))
        return kIOReturnError;

    OSString *comment = OSDynamicCast(OSString, action->getObject("Comment"));
    const char *commentCString = nullptr;
    if (comment)
        commentCString = comment->getCStringNoCopy();
    
    SYSLOG("cdhs", "Added platform binary override: '%s'", commentCString ? commentCString : "unknown");
    
    return kIOReturnSuccess;
}
