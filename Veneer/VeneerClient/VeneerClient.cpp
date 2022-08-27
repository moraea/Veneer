//
//  VeneerClient.cpp
//  Veneer
//
//  Created by flagers on 4/20/22.
//  Copyright © 2022 flagers. All rights reserved.
//

// Kernel SDK headers
#include <IOKit/IOLib.h>
// Lilu headers
#include <Headers/kern_api.hpp>
// Project headers
#include "VeneerClient.hpp"

OSDefineMetaClassAndStructors(VeneerClient, IOUserClient);

// User client dispatch table
const IOExternalMethodDispatch VeneerClient::sMethods[kNumberOfMethods] = {
    { //kMethodOpen
        (IOExternalMethodAction) &VeneerClient::sMethodOpen,      // Method pointer
        0,                                                          // Num of scalar input values
        0,                                                          // Num of struct input values
        0,                                                          // Num of scalar output values
        0                                                           // Num of struct output values
    },
    { //kMethodClose
        (IOExternalMethodAction) &VeneerClient::sMethodClose,     // Method pointer
        0,                                                          // Num of scalar input values
        0,                                                          // Num of struct input values
        0,                                                          // Num of scalar output values
        0                                                           // Num of struct output values
    },
    { //kMethodOpen
        (IOExternalMethodAction) &VeneerClient::sMethodTest,      // Method pointer
        0,                                                          // Num of scalar input values
        0,                                                          // Num of struct input values
        0,                                                          // Num of scalar output values
        0                                                           // Num of struct output values
    }
};

// IOUserClient overrides
IOReturn VeneerClient::externalMethod(uint32_t selector, IOExternalMethodArguments *arguments, IOExternalMethodDispatch *dispatch, OSObject *target, void *reference)
{
    if (selector >= kNumberOfMethods)
        return kIOReturnUnsupported;
    
    dispatch = (IOExternalMethodDispatch*) &sMethods[selector];
    target = this;
    reference = NULL;
    
    return super::externalMethod(selector, arguments, dispatch, target, reference);
}

bool VeneerClient::initWithTask(task_t owningTask, void *securityToken, UInt32 type, OSDictionary *properties)
{
    if (!owningTask)
        return false;
    
    if (!super::initWithTask(owningTask, securityToken, type))
        return false;
    
    mTask = owningTask;
    mProvider = NULL;
    
    return true;
}

bool VeneerClient::start(IOService *provider)
{
    mProvider = provider;
    
    return super::start(provider);
}

void VeneerClient::stop(IOService* provider)
{
    super::stop(provider);
}

IOReturn VeneerClient::clientClose(void)
{
    if (!isInactive())
        terminate();
    
    return kIOReturnSuccess;
}

// Static dispatch methods
IOReturn VeneerClient::sMethodOpen(VeneerClient *target, void *ref, IOExternalMethodArguments *args)
{
    return target->methodOpen(args);
}

IOReturn VeneerClient::sMethodClose(VeneerClient *target, void *ref, IOExternalMethodArguments *args)
{
    return target->methodClose(args);
}

IOReturn VeneerClient::sMethodTest(VeneerClient *target, void *ref, IOExternalMethodArguments *args)
{
    return target->methodTest(args);
}

// Non-static driver methods
IOReturn VeneerClient::methodOpen(IOExternalMethodArguments *args)
{
    return kIOReturnSuccess;
}

IOReturn VeneerClient::methodClose(IOExternalMethodArguments *args)
{
    return kIOReturnSuccess;
}

IOReturn VeneerClient::methodTest(IOExternalMethodArguments *args)
{
    return kIOReturnSuccess;
}
