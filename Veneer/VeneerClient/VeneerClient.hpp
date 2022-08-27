//
//  VeneerClient.hpp
//  Veneer
//
//  Created by flagers on 8/26/22.
//  Copyright © 2022 flagers. All rights reserved.
//

#ifndef VeneerClient_hpp
#define VeneerClient_hpp

// Kernel SDK headers
#include <IOKit/IOService.h>
#include <IOKit/IOUserClient.h>
// Project headers
#include "UserKernelShared.h"

class VeneerClient : public IOUserClient
{
    using super = IOUserClient;
    OSDeclareDefaultStructors(VeneerClient);
    
private:
    IOService *mProvider;
    task_t mTask;
    static const IOExternalMethodDispatch sMethods[kNumberOfMethods];
    
public:
    virtual void stop(IOService* provider) override;
    virtual bool start(IOService* provider) override;
    
    virtual bool initWithTask(task_t owningTask, void* securityToken,
                              UInt32 type, OSDictionary* properties) override;

    virtual IOReturn clientClose(void) override;
    virtual IOReturn externalMethod(uint32_t selector, IOExternalMethodArguments* arguments,
                                    IOExternalMethodDispatch* dispatch, OSObject* target, void* reference) override;
    
protected:
    static IOReturn sMethodOpen(VeneerClient* target, void* ref, IOExternalMethodArguments* args);
    static IOReturn sMethodClose(VeneerClient* target, void* ref, IOExternalMethodArguments* args);
    static IOReturn sMethodTest(VeneerClient* target, void* ref, IOExternalMethodArguments* args);
    
    virtual IOReturn methodOpen(IOExternalMethodArguments* args);
    virtual IOReturn methodClose(IOExternalMethodArguments* args);
    virtual IOReturn methodTest(IOExternalMethodArguments* args);
};

#endif /* VeneerClient_hpp */
