#include "WireframeAllocator.h"
#include <stdlib.h>

// Structs
typedef struct WfaHostAllocationInfo {
    void* unalignedMemory;
} WfaHostAllocationInfo;

// Allocation function declarations
static void* VKAPI_CALL wfaAllocationFunction(void* pUserData, size_t size, size_t alignment, VkSystemAllocationScope allocationScope);
static void* VKAPI_CALL wfaReallocationFunction(void* pUserData, void* pOriginal, size_t size, size_t alignment, VkSystemAllocationScope allocationScope);
static void  VKAPI_CALL wfaFreeFunction(void* pUserData, void* pMemory);
static void  VKAPI_CALL wfaInternalAllocationNotification(void* pUserData, size_t size, VkInternalAllocationType allocationType, VkSystemAllocationScope allocationScope);
static void  VKAPI_CALL wfaInternalFreeNotification(void* pUserData, size_t size, VkInternalAllocationType allocationType, VkSystemAllocationScope allocationScope);

// Variables
VkAllocationCallbacks callbacks = {
    NULL,
    wfaAllocationFunction,
    wfaReallocationFunction,
    wfaFreeFunction,
    wfaInternalAllocationNotification,
    wfaInternalFreeNotification
};

// Allocation function definitions
static void* VKAPI_CALL wfaAllocationFunction(void* pUserData, size_t size, size_t alignment, VkSystemAllocationScope allocationScope) {
    if(!size)
        return NULL;

    // Calculate the needed offset for the aligned memory
    size_t offset = alignment - 1 + sizeof(WfaHostAllocationInfo);

    // Allocate all needed memory
    WfaHostAllocationInfo allocInfo;
    allocInfo.unalignedMemory = malloc(size + offset);

    if(!allocInfo.unalignedMemory)
        return NULL;
        
    // Set the aligned memory
    void* alignedMemory = (void*)(((size_t)allocInfo.unalignedMemory + offset) & ~(alignment - 1));
        
    // Copy the host allocation info to the back of the aligned memory
    ((WfaHostAllocationInfo*)alignedMemory)[-1] = allocInfo;

    return alignedMemory;
}
static void* VKAPI_CALL wfaReallocationFunction(void* pUserData, void* pOriginal, size_t size, size_t alignment, VkSystemAllocationScope allocationScope) {
    if(!size) {
        wfaFreeFunction(pUserData, pOriginal);
        return NULL;
    }

    // Get the host allocation info
    WfaHostAllocationInfo allocInfo = ((WfaHostAllocationInfo*)pOriginal)[-1];

    // Calculate the needed offset for the aligned memory
    size_t offset = alignment - 1 + sizeof(WfaHostAllocationInfo);

    // Reallocate the unaligned memory
    allocInfo.unalignedMemory = realloc(allocInfo.unalignedMemory, size + offset);
    if(!allocInfo.unalignedMemory)
        return NULL;
 
    // Set the aligned memory
    void* alignedMemory = (void*)(((size_t)allocInfo.unalignedMemory + offset) & ~(alignment - 1));

    // Copy the host allocation info to the back of the aligned memory
    ((WfaHostAllocationInfo*)alignedMemory)[-1] = allocInfo;

    return alignedMemory;
}
static void  VKAPI_CALL wfaFreeFunction(void* pUserData, void* pMemory) {
    if(!pMemory)
        return;
        
    // Get the host allocation info
    WfaHostAllocationInfo allocInfo = ((WfaHostAllocationInfo*)pMemory)[-1];

    // Free the unaligned memory
    free(allocInfo.unalignedMemory);
}
static void  VKAPI_CALL wfaInternalAllocationNotification(void* pUserData, size_t size, VkInternalAllocationType allocationType, VkSystemAllocationScope allocationScope) {

}
static void  VKAPI_CALL wfaInternalFreeNotification(void* pUserData, size_t size, VkInternalAllocationType allocationType, VkSystemAllocationScope allocationScope) {

}

// Public functions
VkAllocationCallbacks* wfaGetVulkanAllocationCallbacks() {
    return &callbacks;
}
PFN_vkAllocationFunction wfaGetVulkanAllocationFunctionPtr() {
    return wfaAllocationFunction;
}
PFN_vkReallocationFunction wfaGetVulkanReallocationFunctionPtr() {
    return wfaReallocationFunction;
}
PFN_vkFreeFunction wfaGetVulkanFreeFunctionPtr() {
    return wfaFreeFunction;
}
PFN_vkInternalAllocationNotification wfaGetVulkanInternalAllocationNotificationPtr() {
    return wfaInternalAllocationNotification;
}
PFN_vkInternalFreeNotification wfaGetVulkanInternalFreeNotificationPtr() {
    return wfaInternalFreeNotification;
}