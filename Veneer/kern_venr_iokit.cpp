//
//  kern_venr_iokit.cpp
//  Veneer
//
//  Created by flagers on 8/26/22.
//  Copyright © 2022 flagers. All rights reserved.
//

// Kernel SDK headers
#include <IOKit/IOService.h>
// Lilu headers
#include <Headers/plugin_start.hpp>
#include <Headers/kern_version.hpp>
// Project headers
#include "kern_venr_iokit.hpp"

OSDefineMetaClassAndStructors(Veneer, IOService)

Veneer *ADDPR(selfInstance) = nullptr;

IOService *Veneer::probe(IOService *provider, SInt32 *score) {
    ADDPR(selfInstance) = this;
    setProperty("VersionInfo", kextVersion);
    auto service = IOService::probe(provider, score);
    return ADDPR(startSuccess) ? service : nullptr;
}

bool Veneer::start(IOService *provider) {
    ADDPR(selfInstance) = this;
    if (!IOService::start(provider)) {
        SYSLOG("init", "failed to start the parent");
        return false;
    }
    registerService();

    return ADDPR(startSuccess);
}

void Veneer::stop(IOService *provider) {
    ADDPR(selfInstance) = nullptr;
    IOService::stop(provider);
}
