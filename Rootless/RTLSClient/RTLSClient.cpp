//
//  RTLSClient.cpp
//  Rootless
//
//  Created by flagers on 4/20/22.
//  Copyright © 2022 flagers. All rights reserved.
//

// Kernel SDK headers
#include <IOKit/IOLib.h>
// Lilu headers
#include <Headers/kern_api.hpp>
// Project headers
#include "RTLSClient.hpp"

OSDefineMetaClassAndStructors(RTLSClient, IOUserClient);

// User client dispatch table
const IOExternalMethodDispatch RTLSClient::sMethods[kNumberOfMethods] = {
    { //kMethodOpen
        (IOExternalMethodAction) &RTLSClient::sMethodOpen,      // Method pointer
        0,                                                          // Num of scalar input values
        0,                                                          // Num of struct input values
        0,                                                          // Num of scalar output values
        0                                                           // Num of struct output values
    },
    { //kMethodClose
        (IOExternalMethodAction) &RTLSClient::sMethodClose,     // Method pointer
        0,                                                          // Num of scalar input values
        0,                                                          // Num of struct input values
        0,                                                          // Num of scalar output values
        0                                                           // Num of struct output values
    },
    { //kMethodOpen
        (IOExternalMethodAction) &RTLSClient::sMethodTest,      // Method pointer
        0,                                                          // Num of scalar input values
        0,                                                          // Num of struct input values
        0,                                                          // Num of scalar output values
        0                                                           // Num of struct output values
    }
};

// IOUserClient overrides
IOReturn RTLSClient::externalMethod(uint32_t selector, IOExternalMethodArguments *arguments, IOExternalMethodDispatch *dispatch, OSObject *target, void *reference)
{
    if (selector >= kNumberOfMethods)
        return kIOReturnUnsupported;
    
    dispatch = (IOExternalMethodDispatch*) &sMethods[selector];
    target = this;
    reference = NULL;
    
    return super::externalMethod(selector, arguments, dispatch, target, reference);
}

bool RTLSClient::initWithTask(task_t owningTask, void *securityToken, UInt32 type, OSDictionary *properties)
{
    if (!owningTask)
        return false;
    
    if (!super::initWithTask(owningTask, securityToken, type))
        return false;
    
    mTask = owningTask;
    mProvider = NULL;
    
    return true;
}

bool RTLSClient::start(IOService *provider)
{
    bool success;
    
    mProvider = OSDynamicCast(RTLSClientProvider, provider);
    success = (mProvider != NULL);
    
    if (success)
        success = super::start(provider);
    
    return success;
}

void RTLSClient::stop(IOService* provider)
{
    super::stop(provider);
}

IOReturn RTLSClient::clientClose(void)
{
    if (!isInactive())
        terminate();
    
    return kIOReturnSuccess;
}

// Static dispatch methods
IOReturn RTLSClient::sMethodOpen(RTLSClient *target, void *ref, IOExternalMethodArguments *args)
{
    return target->methodOpen(args);
}

IOReturn RTLSClient::sMethodClose(RTLSClient *target, void *ref, IOExternalMethodArguments *args)
{
    return target->methodClose(args);
}

IOReturn RTLSClient::sMethodTest(RTLSClient *target, void *ref, IOExternalMethodArguments *args)
{
    return target->methodTest(args);
}

// Non-static driver methods
IOReturn RTLSClient::methodOpen(IOExternalMethodArguments *args)
{
    return kIOReturnSuccess;
}

IOReturn RTLSClient::methodClose(IOExternalMethodArguments *args)
{
    return kIOReturnSuccess;
}

IOReturn RTLSClient::methodTest(IOExternalMethodArguments *args)
{
    return kIOReturnSuccess;
}
