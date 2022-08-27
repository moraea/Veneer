//
//  kern_venr.cpp
//  Veneer
//
//  Created by flagers on 8/26/22.
//  Copyright © 2022 flagers. All rights reserved.
//

// Lilu headers
#include <Headers/kern_api.hpp>
// Project headers
#include "kern_venr.hpp"

static VeneerLilu veneer;

VeneerLilu* VeneerLilu::callbackVenr = nullptr;

void VeneerLilu::createShared() {
    if (callbackVenr)
        PANIC(MODULE_SHORT, "Attempted to assign Veneer callback again");
    
    callbackVenr = &veneer;
    
    if (!callbackVenr)
        PANIC(MODULE_SHORT, "Failed to assign Veneer callback");
}

void VeneerLilu::init() {

}

void VeneerLilu::deinit() {

}
