//
//  kern_start.cpp
//  Rootless
//
//  Copyright © 2021-2022 flagers. All rights reserved.
//

// Lilu headers
#include <Headers/plugin_start.hpp>
#include <Headers/kern_api.hpp>
// Project headers
#include "kern_rtls.hpp"

// Boot args.
static const char *bootargOff[] {
    "-rtlsoff"
};
static const char *bootargDebug[] {
    "-rtlsdbg"
};
static const char *bootargBeta[] {
    "-rtlsbeta"
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
    KernelVersion::Monterey,
    []() {
        RTLS::createShared();
        RTLS::getShared()->init();
    }
};
