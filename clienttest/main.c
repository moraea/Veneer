//
//  main.c
//  clienttest
//
//  Created by flagers on 4/20/22.
//  Copyright © 2022 flagers. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <IOKit/IOKitLib.h>

#include "UserKernelShared.h"

int main(int argc, const char * argv[]) {
    kern_return_t   result;
    io_service_t    service;
    io_iterator_t   iterator;
    io_connect_t    connect;
    bool            found = false;
    
    result = IOServiceGetMatchingServices(kIOMasterPortDefault, IOServiceMatching("RTLSClientProvider"), &iterator);
    
    if (result != KERN_SUCCESS) {
        fprintf(stderr, "IOServiceGetMatchingServices returned 0x%08x\n\n", result);
        return -1;
    }
        
    while ((service = IOIteratorNext(iterator)) != IO_OBJECT_NULL) {
        printf("Found an instance of RTLSClientProvider.\n");
        found = true;
        
        result = IOServiceOpen(service, mach_task_self(), 0, &connect);
        if (result != kIOReturnSuccess)
            break;
        
        IOObjectRelease(service);
    }
    
    // Release the io_iterator_t now that we're done with it.
    IOObjectRelease(iterator);
    
    if (!found) {
        fprintf(stderr, "Could not find any instances of RTLSClientProvider.\n");
        return -1;
    }
    
    // Now execute any method you want, like:
    printf("Calling functions from userspace to kernel space...\n\n");
    for (int i = 0; i < kNumberOfMethods; i++) {
        result = IOConnectCallScalarMethod(connect, i, NULL, 0, NULL, NULL);
        if (result == kIOReturnSuccess)
            printf("Successfully called method (%d) in kernel space.\n", i);
        sleep(1.5);
    }

    return EXIT_SUCCESS;
}
