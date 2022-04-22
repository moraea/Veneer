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

}

void RTLS::deinit() {

}
