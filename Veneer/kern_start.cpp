//
//  kern_start.cpp
//  Veneer
//
//  Copyright © 2021-2022 flagers. All rights reserved.
//

// Lilu headers
#include <Headers/plugin_start.hpp>
#include <Headers/kern_api.hpp>
// Project headers
#include "kern_venr.hpp"

// Boot args.
static const char *bootargOff[] {
    "-venroff"
};
static const char *bootargDebug[] {
    "-venrdbg"
};
static const char *bootargBeta[] {
    "-venrbeta"
};

// Plugin configuration.
PluginConfiguration ADDPR(config) {
    xStringify(PRODUCT_NAME),
    parseModuleVersion(xStringify(MODULE_VERSION)),
    LiluAPI::AllowNormal,
    bootargOff,
    arrsize(bootargOff),
    bootargDebug,
    arrsize(bootargDebug),
    bootargBeta,
    arrsize(bootargBeta),
    KernelVersion::BigSur,
    KernelVersion::Ventura,
    []() {
        VeneerLilu::createShared();
        VeneerLilu::getShared()->init();
    }
};
