//
//  kern_venr.hpp
//  Veneer
//
//  Created by flagers on 8/26/22.
//  Copyright © 2022 flagers. All rights reserved.
//

#ifndef kern_venr_hpp
#define kern_venr_hpp

#define MODULE_SHORT "venr"

class VeneerLilu final {
public:
    void init();
    void deinit();
    
    /**
     * Allocate single instance for shared usage and callbacks.
     */
    static void createShared();
    
    /**
     * Obtain the allocated shared instance.
     *
     * @return Allocated VENR instance
     */
    static VeneerLilu* getShared() {
        return callbackVenr;
    }
    
private:
    /**
     *  Global instance set in init() for callback access (i.e. access from static functions).
     *  Only instance allowed of this class.
     */
    static VeneerLilu* callbackVenr;
};

#endif /* kern_venr_hpp */
