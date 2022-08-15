#include "WireframeAllocator.hpp"

namespace wfa {
    // Structs
    struct AllocationInfo {
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
        size_t offset = alignment - 1 + sizeof(AllocationInfo);

        // Allocate all needed memory
        AllocationInfo allocInfo;
        allocInfo.unalignedMemory = malloc(size + offset);

        if(!allocInfo.unalignedMemory)
            return nullptr;
        
        // Set the aligned memory
        void* alignedMemory = (void*)(((size_t)allocInfo.unalignedMemory + offset) & ~(alignment - 1));
        
        // Copy the allocation info to the back of the aligned memory
        ((AllocationInfo*)alignedMemory)[-1] = allocInfo;

        return alignedMemory;
    }
    static void* VKAPI_CALL ReallocationFunction(void* pUserData, void* pOriginal, size_t size, size_t alignment, VkSystemAllocationScope allocationScope) {
        if(!size) {
            FreeFunction(pUserData, pOriginal);
            return nullptr;
        }

        // Get the allocation info
        AllocationInfo allocInfo = ((AllocationInfo*)pOriginal)[-1];

        // Calculate the needed offset for the aligned memory
        size_t offset = alignment - 1 + sizeof(AllocationInfo);

        // Reallocate the unaligned memory
        allocInfo.unalignedMemory = realloc(allocInfo.unalignedMemory, size + offset);
        if(!allocInfo.unalignedMemory)
            return nullptr;
        
        // Set the aligned memory
        void* alignedMemory = (void*)(((size_t)allocInfo.unalignedMemory + offset) & ~(alignment - 1));

        // Copy the allocation info to the back of the aligned memory
        ((AllocationInfo*)alignedMemory)[-1] = allocInfo;

        return 0;
    }
    static void  VKAPI_CALL FreeFunction(void* pUserData, void* pMemory) {
        if(!pMemory)
            return;
        
        // Get the allocation info
        AllocationInfo allocInfo = ((AllocationInfo*)pMemory)[-1];

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
}