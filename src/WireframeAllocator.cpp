#include "WireframeAllocator.hpp"

namespace wfa {
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
        return 0;
    }
    static void* VKAPI_CALL ReallocationFunction(void* pUserData, void* pOriginal, size_t size, size_t alignment, VkSystemAllocationScope allocationScope) {
        return 0;
    }
    static void  VKAPI_CALL FreeFunction(void* pUserData, void* pMemory) {

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