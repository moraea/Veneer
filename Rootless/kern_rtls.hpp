//
//  kern_rtls.hpp
//  Rootless
//
//  Created by flagers on 4/14/22.
//  Copyright © 2022 flagers. All rights reserved.
//

#ifndef kern_rtls_hpp
#define kern_rtls_hpp

#define MODULE_SHORT "rtls"

class RTLS {
public:
    void init();
    void deinit();
    
    /**
     * Alocate single instance for shared usage and callbacks
     */
    static void createShared();
    
    /**
     * Obtain the allocated shared instance
     *
     * @return Allocated RTLS instance
     */
    static RTLS* getShared() {
        return callbackRtls;
    }
    
private:
    /**
     * The only allowed instance of this class
     */
    static RTLS* callbackRtls;
};

#endif /* kern_rtls_hpp */
