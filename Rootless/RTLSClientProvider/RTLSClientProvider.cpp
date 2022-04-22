//
//  RTLSClientProvider.cpp
//  Rootless
//
//  Created by flagers on 4/21/22.
//  Copyright © 2022 flagers. All rights reserved.
//

// Project headers
#include "RTLSClientProvider.hpp"

OSDefineMetaClassAndStructors(RTLSClientProvider, IOService);

bool RTLSClientProvider::start(IOService* provider)
{
    bool    success;
    success = super::start(provider);
    
    if (success)
        registerService();
    
    return success;
}

void RTLSClientProvider::stop(IOService* provider)
{
    super::stop(provider);
}

bool RTLSClientProvider::init(OSDictionary* dictionary)
{
    if (!super::init(dictionary)) {
        return false;
    }
    
    waitForMatchingService(nameMatching("Rootless"));
    rootless = RTLS::getShared();

    return true;
}

void RTLSClientProvider::free(void)
{
    super::free();
}
