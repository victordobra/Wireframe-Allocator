#include "WireframeAllocator.hpp"

namespace wfa {
    // Structs
    struct HostAllocationInfo {
        void* unalignedMemory;
    };

    // Allocation function declarations
    static void* VKAPI_CALL AllocationFunction(void* pUserData, size_t size, size_t alignment, VkSystemAllocationScope allocationScope);
    static void* VKAPI_CALL ReallocationFunction(void* pUserData, void* pOriginal, size_t size, size_t alignment, VkSystemAllocationScope allocationScope);
    static void  VKAPI_CALL FreeFunction(void* pUserData, void* pMemory);
    static void  VKAPI_CALL InternalAllocationNotification(void* pUserData, size_t size, VkInternalAllocationType allocationType, VkSystemAllocationScope allocationScope);
    static void  VKAPI_CALL InternalFreeNotification(void* pUserData, size_t size, VkInternalAllocationType allocationType, VkSystemAllocationScope allocationScope);

    // Variables
    VkAllocationCallbacks callbacks {
        nullptr,
        AllocationFunction,
        ReallocationFunction,
        FreeFunction,
        InternalAllocationNotification,
        InternalFreeNotification
    };

    // Allocation function definitions
    static void* VKAPI_CALL AllocationFunction(void* pUserData, size_t size, size_t alignment, VkSystemAllocationScope allocationScope) {
        if(!size)
            return nullptr;

        // Calculate the needed offset for the aligned memory
        size_t offset = alignment - 1 + sizeof(HostAllocationInfo);

        // Allocate all needed memory
        HostAllocationInfo allocInfo;
        allocInfo.unalignedMemory = malloc(size + offset);

        if(!allocInfo.unalignedMemory)
            return nullptr;
        
        // Set the aligned memory
        void* alignedMemory = (void*)(((size_t)allocInfo.unalignedMemory + offset) & ~(alignment - 1));
        
        // Copy the host allocation info to the back of the aligned memory
        ((HostAllocationInfo*)alignedMemory)[-1] = allocInfo;

        return alignedMemory;
    }
    static void* VKAPI_CALL ReallocationFunction(void* pUserData, void* pOriginal, size_t size, size_t alignment, VkSystemAllocationScope allocationScope) {
        if(!size) {
            FreeFunction(pUserData, pOriginal);
            return nullptr;
        }

        // Get the host allocation info
        HostAllocationInfo allocInfo = ((HostAllocationInfo*)pOriginal)[-1];

        // Calculate the needed offset for the aligned memory
        size_t offset = alignment - 1 + sizeof(HostAllocationInfo);

        // Reallocate the unaligned memory
        allocInfo.unalignedMemory = realloc(allocInfo.unalignedMemory, size + offset);
        if(!allocInfo.unalignedMemory)
            return nullptr;
        
        // Set the aligned memory
        void* alignedMemory = (void*)(((size_t)allocInfo.unalignedMemory + offset) & ~(alignment - 1));

        // Copy the host allocation info to the back of the aligned memory
        ((HostAllocationInfo*)alignedMemory)[-1] = allocInfo;

        return 0;
    }
    static void  VKAPI_CALL FreeFunction(void* pUserData, void* pMemory) {
        if(!pMemory)
            return;
        
        // Get the host allocation info
        HostAllocationInfo allocInfo = ((HostAllocationInfo*)pMemory)[-1];

        // Free the unaligned memory
        free(allocInfo.unalignedMemory);
    }
    static void  VKAPI_CALL InternalAllocationNotification(void* pUserData, size_t size, VkInternalAllocationType allocationType, VkSystemAllocationScope allocationScope) {

    }
    static void  VKAPI_CALL InternalFreeNotification(void* pUserData, size_t size, VkInternalAllocationType allocationType, VkSystemAllocationScope allocationScope) {

    }

    // Public functions
    VkAllocationCallbacks* GetAllocationCallbacks() {
        return &callbacks;
    }
    PFN_vkAllocationFunction GetAllocationFunctionPtr() {
        return AllocationFunction;
    }
    PFN_vkReallocationFunction GetReallocationFunctionPtr() {
        return ReallocationFunction;
    }
    PFN_vkFreeFunction GetFreeFunctionPtr() {
        return FreeFunction;
    }
    PFN_vkInternalAllocationNotification GetInternalAllocationNotificationPtr() {
        return InternalAllocationNotification;
    }
    PFN_vkInternalFreeNotification GetInternalFreeNotificationPtr() {
        return InternalFreeNotification
    }
}