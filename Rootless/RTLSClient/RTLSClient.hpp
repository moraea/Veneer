//
//  RTLSClient.hpp
//  Rootless
//
//  Created by flagers on 4/20/22.
//  Copyright © 2022 flagers. All rights reserved.
//

#ifndef RTLSClient_hpp
#define RTLSClient_hpp

#include "UserKernelShared.h"
#include <IOKit/IOService.h>
#include <IOKit/IOUserClient.h>
#include "RTLSClientProvider.hpp"

class RTLSClient : public IOUserClient
{
    using super = IOUserClient;
    OSDeclareDefaultStructors(RTLSClient);
    
private:
    RTLSClientProvider *mProvider;
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
    static IOReturn sMethodOpen(RTLSClient* target, void* ref, IOExternalMethodArguments* args);
    static IOReturn sMethodClose(RTLSClient* target, void* ref, IOExternalMethodArguments* args);
    static IOReturn sMethodTest(RTLSClient* target, void* ref, IOExternalMethodArguments* args);
    
    virtual IOReturn methodOpen(IOExternalMethodArguments* args);
    virtual IOReturn methodClose(IOExternalMethodArguments* args);
    virtual IOReturn methodTest(IOExternalMethodArguments* args);
};


#endif /* RootlessClient_hpp */
