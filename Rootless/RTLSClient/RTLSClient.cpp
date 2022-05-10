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
    { // kMethodOpen
        (IOExternalMethodAction) &RTLSClient::sMethodTest,
        0,
        0,
        0,
        0
    },
    { // kMethodExecutePlist
        (IOExternalMethodAction) &RTLSClient::sMethodPlistExecute,
        0,
        kIOUCVariableStructureSize,
        0,
        0
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
IOReturn RTLSClient::sMethodTest(RTLSClient *target, void *ref, IOExternalMethodArguments *args)
{
    return target->methodTest(args);
}

// Non-static driver methods
IOReturn RTLSClient::methodTest(IOExternalMethodArguments *args)
{
    return kIOReturnSuccess;
}

IOReturn RTLSClient::sMethodPlistExecute(RTLSClient *target, void *ref, IOExternalMethodArguments *args)
{
    IOReturn result;
    IOMemoryMap *map = nullptr;
    const void *inputPtr;
    size_t inputSize;
    
    if (args->structureInputDescriptor != nullptr) {
        map = args->structureInputDescriptor->createMappingInTask(kernel_task, 0, kIOMapAnywhere | kIOMapReadOnly);
        
        if (!map)
            return kIOReturnError;
        
        inputPtr = reinterpret_cast<decltype(inputPtr)>(map->getAddress());
        inputSize = map->getSize();
    } else {
        inputPtr = args->structureInput;
        inputSize = args->structureInputSize;
    }
    OSData *input = OSData::withBytes(inputPtr, inputSize);
    OSSafeReleaseNULL(map);
    if (!input)
        return kIOReturnError;
    
    OSArray *inputXML = OSDynamicCast(OSArray, OSUnserializeXML((const char *)input->getBytesNoCopy(), inputSize));
    if (!inputXML)
        return kIOReturnError;
    
    result = target->plistExecute(inputXML);
    
    inputXML->release();
    input->release();
    return result;
}

// Non-static driver methods
IOReturn RTLSClient::plistExecute(OSArray *plist)
{
    OSCollectionIterator *iterator;
    OSDictionary *actionDict;
    OSString *actionName;
    
    if (NULL != (iterator = OSCollectionIterator::withCollection(plist))) {
        while (NULL != (actionDict = OSDynamicCast(OSDictionary, iterator->getNextObject()))) {
            actionName = OSDynamicCast(OSString, actionDict->getObject("Action"));
            if (!actionName)
                continue;
            
            if (actionName->isEqualTo("Make CDHash a Platform Binary")) {
                mProvider->codesign->cdhash.addCdhashAction(actionDict);
            } else if (actionName->isEqualTo("Create Fake File or Folder")) {
                SYSLOG("rtlsclnt", "unimplemented action (Create Fake File or Folder)");
                //rvplFilesys->
            } else if (actionName->isEqualTo("Add Entitlements for CDHash")) {
                SYSLOG("rtlsclnt", "unimplemented action (Add Entitlements for CDHash)");
                //rvplCodesign->
            } else if (actionName->isEqualTo("Firmlink File or Folder")) {
                SYSLOG("rtlsclnt", "unimplemented action (Firmlink File or Folder)");
                //rvplFilesys->
            } else if (actionName->isEqualTo("SysKC Is Ready")) {
                SYSLOG("rtlsclnt", "unimplemented action (SysKC Is Ready)");
                //rvplFilesys->
            }
        }
    }
    return kIOReturnSuccess;
}
