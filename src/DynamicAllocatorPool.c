#include "WireframeAllocator.h"
#include <stdlib.h>

typedef struct WfaDynamicAllocatorPoolInfo {
    size_t allocatorMaxCount;
    size_t allocatorSize;
    size_t allocatorFreeListMaxSize;
    size_t allocatorCount;
    WfaDynamicAllocator* allocators;
} WfaDynamicAllocatorPoolInfo;

WfaResult wfaCreateDynamicAllocatorPool(const WfaDynamicAllocatorPoolCreateInfo* pDynamicAllocatorPoolInfo, WfaDynamicAllocatorPool* pDynamicAllocatorPool) {
    // Check if all args are valid
    if(!pDynamicAllocatorPoolInfo || !pDynamicAllocatorPool || !pDynamicAllocatorPoolInfo->allocatorMaxCount || !pDynamicAllocatorPoolInfo->allocatorSize || !pDynamicAllocatorPoolInfo->allocatorFreeListMaxSize)
        return WFA_ERROR_INVALID_ARGS;
    
    // Allocate all required memory
    *pDynamicAllocatorPool = malloc(sizeof(WfaDynamicAllocatorPoolInfo) + pDynamicAllocatorPoolInfo->allocatorMaxCount * sizeof(WfaDynamicAllocator));
    if(!*pDynamicAllocatorPool)
        return WFA_ERROR_OUT_OF_MEMORY;
    
    // Set the dynamic allocator pool info
    WfaDynamicAllocatorPoolInfo* info = (WfaDynamicAllocatorPoolInfo*)*pDynamicAllocatorPool;

    info->allocatorMaxCount = pDynamicAllocatorPoolInfo->allocatorMaxCount;
    info->allocatorSize = pDynamicAllocatorPoolInfo->allocatorSize;
    info->allocatorFreeListMaxSize = pDynamicAllocatorPoolInfo->allocatorFreeListMaxSize;
    info->allocatorCount = 0;
    info->allocators = (WfaDynamicAllocator*)((char*)*pDynamicAllocatorPool + sizeof(WfaDynamicAllocatorPoolInfo));

    return WFA_SUCCESS;
}
WfaResult wfaDynamicAllocatorPoolAlloc(WfaDynamicAllocatorPool dynamicAllocatorPool, size_t size, size_t alignment, WfaFreeMemoryFindType freeMemoryFindType, void** ppResult) {
    // Check if all args are valid
    if(!dynamicAllocatorPool || !size || !alignment || (freeMemoryFindType != WFA_FREE_MEMORY_FIND_FIRST_FIT && freeMemoryFindType != WFA_FREE_MEMORY_FIND_BEST_FIT) || !ppResult)
        return WFA_ERROR_INVALID_ARGS;
    
    // Get the dynamic allocator pool info
    WfaDynamicAllocatorPoolInfo* info = (WfaDynamicAllocatorPoolInfo*)dynamicAllocatorPool;

    // Check if the memory block is too large to allocate
    if(size > info->allocatorSize) {
        *ppResult = _aligned_malloc(size, alignment);

        return *ppResult ? WFA_SUCCESS : WFA_ERROR_OUT_OF_MEMORY;
    }

    // Loop through every allocator and try to allocate the requested block of memory
    WfaDynamicAllocator* allocator = info->allocators;
    for(size_t i = 0; i < info->allocatorCount; ++i) {
        WfaResult result = wfaDynamicAllocatorAlloc(dynamicAllocatorPool, size, alignment, freeMemoryFindType, ppResult);
        if(result != WFA_ERROR_OUT_OF_MEMORY && result != WFA_ERROR_FREE_LIST_FULL)
            return result;

        ++allocator;
    }

    // No space was left for the allocator; create a new one
    if(info->allocatorCount == info->allocatorMaxCount)
        return WFA_ERROR_DYNAMIC_ALLOCATOR_POOL_FULL;
    
    // Set the dynamic allocator info
    WfaDynamicAllocatorCreateInfo createInfo;

    createInfo.size = info->allocatorSize;
    createInfo.freeListMaxSize = info->allocatorFreeListMaxSize;

    WfaResult result = wfaCreateDynamicAllocator(&createInfo, info->allocators + info->allocatorCount);
    if(result != WFA_SUCCESS)
        return result;
    
    return wfaDynamicAllocatorAlloc(info->allocators[info->allocatorCount++], size, alignment, freeMemoryFindType, ppResult);
}
WfaResult wfaDynamicAllocatorPoolRealloc(WfaDynamicAllocatorPool dynamicAllocatorPool, size_t size, size_t alignment, WfaFreeMemoryFindType freeMemoryFindType, void** ppMemory) {
    // Check if all args are valid
    if(!dynamicAllocatorPool || !size || !alignment || (freeMemoryFindType != WFA_FREE_MEMORY_FIND_FIRST_FIT && freeMemoryFindType != WFA_FREE_MEMORY_FIND_BEST_FIT) || !ppMemory)
        return WFA_ERROR_INVALID_ARGS;
    
    // Get the dynamic allocator pool info
    WfaDynamicAllocatorPoolInfo* info = (WfaDynamicAllocatorPoolInfo*)dynamicAllocatorPool;

    // Loop through every allocator and try to allocate the requested block of memory
    WfaDynamicAllocator* allocator = info->allocators;

    for(size_t i = 0; i < info->allocatorCount; ++i) {
        WfaResult result = wfaDynamicAllocatorRealloc(dynamicAllocatorPool, size, alignment, freeMemoryFindType, ppMemory);
        if(result == WFA_ERROR_OUT_OF_MEMORY || result == WFA_ERROR_FREE_LIST_FULL)
            break;
        else if(result != WFA_ERROR_MEMORY_OUT_OF_RANGE)
            return result;

        ++allocator;
    }

    // Free the memory from the allocator with the memory in range
    if(allocator != info->allocators + info->allocatorCount) {
        WfaResult result = wfaDynamicAllocatorFree(*allocator, *ppMemory);
        if(result != WFA_SUCCESS)
            return result;
    } 

    // Allocate the memory in another allocator
    void* oldMemory = *ppMemory;

    WfaResult result = wfaDynamicAllocatorPoolAlloc(dynamicAllocatorPool, size, alignment, freeMemoryFindType, ppMemory);
    if(result != WFA_SUCCESS)
        return result;
    
    if(allocator != info->allocators + info->allocatorCount)
        _aligned_free(oldMemory);
    
    return WFA_SUCCESS;
}
WfaResult wfaDynamicAllocatorPoolFree(WfaDynamicAllocatorPool dynamicAllocatorPool, void* pMemory) {
    // Check if all args are valid
    if(!dynamicAllocatorPool || !pMemory)
        return WFA_ERROR_INVALID_ARGS;
    
    // Get the dynamic allocator pool info
    WfaDynamicAllocatorPoolInfo* info = (WfaDynamicAllocatorPoolInfo*)dynamicAllocatorPool;

    // Loop through every allocator and try to allocate the requested block of memory
    WfaDynamicAllocator* allocator = info->allocators;

    for(size_t i = 0; i < info->allocatorCount; ++i) {
        WfaResult result = wfaDynamicAllocatorFree(*allocator, pMemory);
        if(result != WFA_ERROR_MEMORY_OUT_OF_RANGE)
            return result;

        ++allocator;
    }

    // The memory was allocated outsize of any allocator; free it using the same method
    _aligned_free(pMemory);

    return WFA_SUCCESS;
}
void wfaDestroyDynamicAllocatorPoolPool(WfaDynamicAllocatorPool dynamicAllocatorPool) {
    // Get the dynamic allocator pool info
    WfaDynamicAllocatorPoolInfo* info = (WfaDynamicAllocatorPoolInfo*)dynamicAllocatorPool;

    // Destroy every dynamic allocator
    WfaDynamicAllocator* allocator = info->allocators;
    
    for(size_t i = 0; i < info->allocatorCount; ++i) {
        wfaDestroyDynamicAllocator(allocator);
        ++allocator;
    }

    // Free all allocated memory
    free(dynamicAllocatorPool);
}