#ifndef _WFA_H
#define _WFA_H

#include "vulkan.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define WFA_DEFINE_HANDLE(type) typedef struct type ## _T* type;

WFA_DEFINE_HANDLE(WfaDynamicAllocator)

typedef enum WfaResult {
    WFA_SUCCESS,
    WFA_ERROR_INVALID_ARGS,
    WFA_ERROR_OUT_OF_MEMORY,
    WFA_ERROR_FREE_LIST_FULL
} WfaResult;

typedef enum WfaFreeMemoryFindType {
    WFA_FREE_MEMORY_FIND_FIRST_FIT,
    WFA_FREE_MEMORY_FIND_BEST_FIT
} WfaFreeMemoryFindType;

typedef struct WfaFreeListItem {
    size_t offset;
    size_t size;
    struct WfaFreeListItem* pNext;
} WfaFreeListItem;

typedef struct WfaDynamicAllocatorCreateInfo {
    size_t size;
    size_t freeListMaxSize;
} WfaDynamicAllocatorCreateInfo;

/// @brief Creates a dynamic allocator.
/// @param pDynamicAllocatorInfo A pointer to the dynamic allocator info.
/// @param pDynamicAllocator A pointer to the result dynamic allocator.
/// @return WFA_SUCCESS if the operation succeeded, otherwise a corespondign error code.
WfaResult wfaCreateDynamicAllocator(const WfaDynamicAllocatorCreateInfo* pDynamicAllocatorInfo, WfaDynamicAllocator* pDynamicAllocator);
/// @brief Allocates to the given dynamic allocator.
/// @param dynamicAllocator The dynamic allocator to allocate to.
/// @param size The size of the new allocated memory.
/// @param alignment The required alignment for the new allocated memory.
/// @param ppResult A pointer to the pointer to store the resulted allocated memory address in.
/// @return WFA_SUCCESS if the operation succeeded, otherwise a corespondign error code.
WfaResult wfaDynamicAllocatorAlloc(WfaDynamicAllocator dynamicAllocator, size_t size, size_t alignment, WfaFreeMemoryFindType freeMemoryFindType, void** ppResult);
/// @brief Reallocates the given memory in the dynamic allocator.
/// @param dynamicAllocator The dynamic allocator to reallocate from.
/// @param size The new size of the memory.
/// @param alignment The required aligned for the reallocated memory.
/// @param ppMemory A pointer to the pointer in which the memory's address is and where the new address will be written.
/// @return WFA_SUCCESS if the operation succeeded, otherwise a corespondign error code.
WfaResult wfaDynamicAllocatorRealloc(WfaDynamicAllocator dynamicAllocator, size_t size, size_t alignment, WfaFreeMemoryFindType freeMemoryFindType, void** ppMemory);
/// @brief Frees the given memory from the dynamic allocator.
/// @param dynamicAllocator The dynamic allocator to free from.
/// @param pMemory A pointer to the memory to be freed.
/// @return WFA_SUCCESS if the operation succeeded, otherwise a corespondign error code.
WfaResult wfaDynamicAllocatorFree(WfaDynamicAllocator dynamicAllocator, void* pMemory);
/// @brief Destroys the given dynamic allocator.
/// @param dynamicAllocator The dynamic allocator to destroy.
void wfaDestroyDynamicAllocator(WfaDynamicAllocator dynamicAllocator);

#ifndef WFA_NO_VULKAN

/// @brief Returns a pointer to the Vulkan allocation callbacks.
VkAllocationCallbacks* wfaGetVulkanAllocationCallbacks();
/// @brief Returns a pointer to the Vulkan host allocation function.
PFN_vkAllocationFunction wfaGetVulkanAllocationFunctionPtr();
/// @brief Returns a pointer to the Vulkan host reallocation function.
PFN_vkReallocationFunction wfaGetVulkanReallocationFunctionPtr();
/// @brief Returns a pointer to the Vulkan host free function.
PFN_vkFreeFunction wfaGetVulkanFreeFunctionPtr();
/// @brief Returns a pointer to the Vulkan internal allocation notification.
PFN_vkInternalAllocationNotification wfaGetVulkanInternalAllocationNotificationPtr();
/// @brief Returns a po9inter to the Vulkan internal free notification.
PFN_vkInternalFreeNotification wfaGetVulkanInternalFreeNotificationPtr();

#endif

#ifdef __cplusplus
}
#endif

#endif