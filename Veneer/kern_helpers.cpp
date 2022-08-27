//
//  kern_helpers.cpp
//  Veneer
//
//  Created by flagers on 8/26/22.
//  Copyright © 2022 flagers. All rights reserved.
//

#include "kern_helpers.hpp"

void Helpers::cdhashToString(char str[CS_CDHASH_STRING_SIZE], uint8_t const * const cdhash) {
    static char const nibble[] = "0123456789abcdef";
    for (int i = 0; i < CS_CDHASH_LEN; ++i) {
        str[i * 2] = nibble[(cdhash[i] & 0xf0) >> 4];
        str[i * 2 + 1] = nibble[cdhash[i] & 0x0f];
    }
    str[CS_CDHASH_STRING_SIZE - 1] = 0;
}
