//
//  kern_start.cpp
//  Rootless
//
//  Copyright © 2021-2022 flagers. All rights reserved.
//

// Lilu headers
#include <Headers/plugin_start.hpp>
#include <Headers/kern_api.hpp>

#define MODULE_SHORT "rtls"

#pragma mark - Plugin start
static void pluginStart() {
    DBGLOG(MODULE_SHORT, "start");
};

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
    pluginStart
};
