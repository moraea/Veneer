//
//  RTLSClientProvider.hpp
//  Rootless
//
//  Created by flagers on 4/21/22.
//  Copyright © 2022 flagers. All rights reserved.
//

#ifndef RTLSClientProvider_hpp
#define RTLSClientProvider_hpp

// Kernel SDK headers
#include <IOKit/IOService.h>
#include <IOKit/IOLib.h>
// Project headers
#include "UserKernelShared.h"
#include "kern_rtls.hpp"

class RTLSClientProvider : public IOService
{
    using super = IOService;
    friend class RTLSClient;
    OSDeclareDefaultStructors(RTLSClientProvider);
    
public:
    virtual bool init(OSDictionary* dictionary) override;
    virtual void free(void) override;
    
    virtual bool start(IOService* provider) override;
    virtual void stop(IOService* provider) override;

private:
    RTLS::Codesign *codesign;
};

#endif /* RTLSClientProvider_hpp */
