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

// Kernel SDK headers
#include <IOKit/IORegistryEntry.h>
#include <IOKit/IOService.h>
#include <mach-o/loader.h>
#include <mach-o/fat.h>
// Lilu headers
#include <Headers/kern_patcher.hpp>

class RTLS {
    friend class RTLSClientProvider;
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

    struct Codesign {
        void init(KernelPatcher &patcher) {
            cdhash.init(patcher);
        };
    
        struct CDHash {
            void init(KernelPatcher &patcher);

            /* Mykola Grymalyuk code-signing certificate team ID */
            static constexpr const char *mykolaTeamId { "S74BDJXQMD" };

            /* OSArray with OSData instances */
            OSArray *platformCDHashes;

            mach_vm_address_t orgCsfgGetPlatformBinary {};
            static int wrapCsfgGetPlatformBinary(struct fileglob *fg);
            mach_vm_address_t orgCsblobGetPlatformBinary {};
            static int wrapCsblobGetPlatformBinary(struct cs_blob *blob);
            mach_vm_address_t orgCsprocGetPlatformBinary {};
            static int wrapCsprocGetPlatformBinary(struct proc *p);

            /* Pointer to get_bsdtask_info function */
            void *(*orgGetBsdTaskInfo)(task_t t) {nullptr};
            /* Pointer to cs_get_cdhash function */
            uint8_t *(*orgCsGetCdhash)(struct proc *) {nullptr};
            /* Pointer to csproc_get_teamid function */
            const char *(*orgCsprocGetTeamId)(struct proc *) {nullptr};
            /* Pointer to csfg_get_teamid function */
            const char *(*orgCsfgGetTeamId)(struct fileglob *fg) {nullptr};
            /* Pointer to csfg_get_path function */
            int *(*orgCsfgGetPath)(struct fileglob *fg, char *path, int *len) {nullptr};
            /* Pointer to csfg_get_cdhash function */
            uint8_t *(*orgCsfgGetCdhash)(struct fileglob *fg, uint64_t offset, size_t *cdhash_size) {nullptr};
            /* Pointer to csblob_get_teamid function */
            const char *(*orgCsblobGetTeamId)(struct cs_blob *) {nullptr};
            /* Pointer to csblob_get_cdhash function */
            uint8_t *(*orgCsblobGetCdhash)(struct cs_blob *) {nullptr};

            bool verifyFromFileglob(struct fileglob *fg);
            bool cdhashAllowed(uint8_t *cdhashToCheck);
            
            IOReturn addCdhashAction(OSDictionary *action);
            
        private:
            /**
             *  Private self instance for callbacks
             */
            static CDHash *callbackCdhs;
        } cdhash;

    } codesign;
};

#endif /* kern_rtls_hpp */
