//
//  kern_venr_iokit.hpp
//  Veneer
//
//  Created by flagers on 8/26/22.
//  Copyright © 2022 flagers. All rights reserved.
//

#ifndef kern_venr_iokit_hpp
#define kern_venr_iokit_hpp

// Kernel SDK headers
#include <IOKit/IOService.h>
// Lilu headers
#include <Headers/kern_util.hpp>

class EXPORT Veneer : public IOService {
    OSDeclareDefaultStructors(Veneer)
public:
    IOService *probe(IOService *provider, SInt32 *score) override;
    bool start(IOService *provider) override;
    void stop(IOService *provider) override;
};

#endif /* kern_venr_iokit_hpp */
