//
//  kern_helpers.hpp
//  Veneer
//
//  Created by flagers on 8/26/22.
//  Copyright © 2022 flagers. All rights reserved.
//

#ifndef kern_helpers_hpp
#define kern_helpers_hpp

// Kernel SDK headers
#include <sys/kernel_types.h>
// Lilu headers
#include <Headers/kern_util.hpp>

#define CS_CDHASH_LEN 20
#define CS_CDHASH_STRING_SIZE (CS_CDHASH_LEN * 2 + 1)

namespace Helpers {
    EXPORT void cdhashToString(char str[CS_CDHASH_STRING_SIZE], uint8_t const * const cdhash);
}

#endif /* kern_helpers_hpp */
