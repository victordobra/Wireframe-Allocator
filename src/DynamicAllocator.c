#include "WireframeAllocator.h"
#include <stdlib.h>
#include <string.h>

typedef struct WfaDynamicAllocatorInfo {
    size_t size;
    size_t freeListMaxSize;
    WfaFreeListItem* freeListStart;
    WfaFreeListItem* freeListHead;
    void* memory;
} WfaDynamicAllocatorInfo;

WfaResult wfaCreateDynamicAllocator(const WfaDynamicAllocatorCreateInfo* pDynamicAllocatorInfo, WfaDynamicAllocator* pDynamicAllocator) {
    // Check if all args are valid
    if(!pDynamicAllocatorInfo->size || !pDynamicAllocatorInfo->freeListMaxSize)
        return WFA_ERROR_INVALID_ARGS;
    
    // Allocate the required memory
    *pDynamicAllocator = (WfaDynamicAllocator)malloc(sizeof(WfaDynamicAllocatorInfo) + pDynamicAllocatorInfo->freeListMaxSize * sizeof(WfaFreeListItem) + pDynamicAllocatorInfo->size);

    // Set the dynamic allocator info
    WfaDynamicAllocatorInfo* info = (WfaDynamicAllocatorInfo*)*pDynamicAllocator;

    info->size = pDynamicAllocatorInfo->size;
    info->freeListMaxSize = pDynamicAllocatorInfo->freeListMaxSize;
    info->freeListStart = (WfaFreeListItem*)((char*)*pDynamicAllocator + sizeof(WfaDynamicAllocatorInfo));
    info->freeListHead = info->freeListStart;
    info->memory = info->freeListHead + info->freeListMaxSize;

    // Set the first list item's values
    info->freeListHead->size = info->size;
    info->freeListHead->offset = 0;
    info->freeListHead->pNext = NULL;

    // Reset every other free list item's values
    WfaFreeListItem* freeListItem = info->freeListStart + 1;
    for(size_t i = 1; i < info->freeListMaxSize; ++i) {
        freeListItem->offset = 0;
        freeListItem->size = 0;
        freeListItem->pNext = NULL;
        ++freeListItem;
    }

    return WFA_SUCCESS;
}
WfaResult wfaDynamicAllocatorAlloc(WfaDynamicAllocator dynamicAllocator, size_t size, size_t alignment, WfaFreeMemoryFindType freeMemoryFindType, void** ppResult) {
    // Check if all args are valid
    if(!dynamicAllocator || !size || !alignment || !ppResult || (freeMemoryFindType != WFA_FREE_MEMORY_FIND_FIRST_FIT && freeMemoryFindType != WFA_FREE_MEMORY_FIND_BEST_FIT))
        return WFA_ERROR_INVALID_ARGS;

    // Get the dynamic allocator info
    WfaDynamicAllocatorInfo* info = (WfaDynamicAllocatorInfo*)dynamicAllocator;

    WfaFreeListItem* targetItem = NULL,* targetPreviousItem = NULL;
    size_t minDifference = -1; // Used only if freeMemoryFindType is set to WFA_DYNAMIC_ALLOCATOR_FREE_MEMORY_FIND_BEST_FIT

    WfaFreeListItem* currentItem = info->freeListHead,* previousItem = NULL;
    
    // Loop through every list item to find the first/best fit (depending on freeMemoryFindType)
    while(currentItem) {
        // Get the needed padding for alignment
        size_t alignedAddress = ((size_t)info->memory + currentItem->offset + sizeof(size_t) + alignment - 1) & ~(alignment - 1);
        size_t requiredPadding = alignedAddress - (size_t)info->memory - currentItem->offset - sizeof(size_t);

        // Get if the memory can fit in the free space
        if(currentItem->size >= size + sizeof(size_t) + requiredPadding) {
            if(freeMemoryFindType == WFA_FREE_MEMORY_FIND_FIRST_FIT) {
                // Set the target item and exit the function
                targetItem = currentItem;
                break;
            } else {
                // Calculate the difference between the needed size and the total size of the free space
                size_t difference = size + sizeof(size_t) + requiredPadding - currentItem->size;

                // Set the new target item if the current difference is the lowest
                if(difference < minDifference) {
                    targetItem = currentItem;
                    targetPreviousItem = previousItem;
                    minDifference = difference;
                }
            }
        }

        // Move on to the next item
        previousItem = currentItem;
        currentItem = currentItem->pNext;
    }

    // Check if a target item was found
    if(!targetItem)
        return WFA_ERROR_OUT_OF_MEMORY;

    // Check if the block will be entirely allocated
    if(sizeof(size_t) + size == targetItem->size) {
        if(previousItem)
            previousItem->pNext = targetItem->pNext;
        else
            info->freeListHead = targetItem->pNext;

        // Set the target address
        *ppResult = info->memory + targetItem->offset + sizeof(size_t);

        // Store the size in the result
        ((size_t*)*ppResult)[-1] = size;

        // Reset the current item
        targetItem->offset = 0;
        targetItem->size = 0;
        targetItem->pNext = NULL;

        return WFA_SUCCESS;
    }

    // Get the needed padding for alignment
    size_t alignedAddress = ((size_t)info->memory + targetItem->offset + sizeof(size_t) + alignment - 1) & ~(alignment - 1);
    size_t requiredPadding = alignedAddress - (size_t)info->memory - targetItem->offset - sizeof(size_t);
    
    // Check if only the padding will be left
    if(requiredPadding + sizeof(size_t) + size == targetItem->size) {
        // Set the item's size to the padding's size
        targetItem->size = requiredPadding;

        // Set the target address
        *ppResult = info->memory + targetItem->offset + requiredPadding + sizeof(size_t);

        // Store the size in the result
        ((size_t*)*ppResult)[-1] = size;

        return WFA_SUCCESS;
    }

    if(requiredPadding) {
        // Find a new item
        WfaFreeListItem* newItem = info->freeListStart;
        WfaFreeListItem* freeListEnd = info->freeListStart + info->freeListMaxSize;

        while(newItem->size && newItem < freeListEnd)
            ++newItem;

        // Check if an empty item was found
        if(newItem == freeListEnd)
            return WFA_ERROR_FREE_LIST_FULL;
        
        // Split the free item into two free items
        targetItem->size = requiredPadding;

        // Add the new item into the linked list
        newItem->pNext = targetItem->pNext;
        targetItem->pNext = newItem;

        // Set the new item's offset and size
        newItem->offset = targetItem->offset + requiredPadding + sizeof(size_t) + size;
        newItem->size = targetItem->size - requiredPadding - sizeof(size_t) - size;
        
        // Set the item's size to the padding's size
        targetItem->size = requiredPadding;

        // Set the target address
        *ppResult = info->memory + targetItem->offset + requiredPadding + sizeof(size_t);

        // Store the size in the result
        ((size_t*)*ppResult)[-1] = size;

        return WFA_SUCCESS;
    }

    // Set the item's new offset and size
    targetItem->offset += sizeof(size_t) + size;
    targetItem->size -= sizeof(size_t) + size; 

    // Set the target address
    *ppResult = info->memory + targetItem->offset - size;

    // Store the size in the result
    ((size_t*)*ppResult)[-1] = size;

    return WFA_SUCCESS;
}
WfaResult wfaDynamicAllocatorRealloc(WfaDynamicAllocator dynamicAllocator, size_t size, size_t alignment, WfaFreeMemoryFindType freeMemoryFindType, void** ppMemory) {
    if(!dynamicAllocator || !size || !alignment || !ppMemory || (freeMemoryFindType != WFA_FREE_MEMORY_FIND_FIRST_FIT && freeMemoryFindType != WFA_FREE_MEMORY_FIND_BEST_FIT))
        return WFA_ERROR_INVALID_ARGS;

    // Get the dynamic allocator info
    WfaDynamicAllocatorInfo* info = (WfaDynamicAllocatorInfo*)dynamicAllocator;

    // Get the item's original size and offset
    size_t originalSize = ((size_t*)*ppMemory)[-1];
    size_t originalOffset = (size_t)*ppMemory - (size_t)info->memory;

    // Find the closest item after the original memory block
    WfaFreeListItem* currentItem = info->freeListHead,* previousItem = NULL;

    while(currentItem->offset < originalOffset) {
        previousItem = currentItem;
        currentItem = currentItem->pNext;
    }

    // Check if it is at the end of the allocated memory
    if(currentItem->offset == originalOffset + originalSize) {
        // Check if the allocation will take up the entire item
        if(currentItem->size == size - originalSize) {
            // Remove the current item from the linked list
            if(previousItem)
                previousItem->pNext = currentItem->pNext;
            else
                info->freeListHead = currentItem->pNext;

            // Store the size in the result
            ((size_t*)*ppMemory)[-1] = size;

            // Reset the current item
            currentItem->offset = 0;
            currentItem->size = 0;
            currentItem->pNext = NULL;

            return WFA_SUCCESS;
        }

        // Set the current item's new offset and size
        currentItem->offset += sizeof(size_t) + size - originalSize;
        currentItem->size -= sizeof(size_t) + size - originalSize;

        // Store the size in the result
        ((size_t*)*ppMemory)[-1] = size;
        
        return WFA_SUCCESS;
    }

    // Free the old memory
    if(previousItem && previousItem->offset + previousItem->size == originalOffset) {
        // Set the previous item's new size
        previousItem->size += sizeof(size_t) + originalSize;

        if(currentItem->offset == originalOffset + sizeof(size_t) + originalSize) {
            // Remove the current item from the linked list
            previousItem->pNext = currentItem->pNext;

            // Reset the current item
            previousItem->offset = 0;
            previousItem->size = 0;
            previousItem->pNext = NULL;
        }
    } else if(currentItem->offset == originalOffset + sizeof(size_t) + originalSize) {
        // Set the current item's new offset and size
        currentItem->offset -= sizeof(size_t) + originalSize;
        currentItem->size += sizeof(size_t) + originalSize;
    } else {
        // Find a new item
        WfaFreeListItem* newItem = info->freeListStart;
        WfaFreeListItem* freeListEnd = info->freeListStart + info->freeListMaxSize;

        while(newItem->size && newItem < freeListEnd)
            ++newItem;

        if(newItem == freeListEnd)
            return WFA_ERROR_FREE_LIST_FULL;
        
        // Add the new item to the linked list
        if(previousItem)
            previousItem->pNext = newItem;
        else
            info->freeListHead = newItem;
        
        newItem->pNext = currentItem;
    }

    // Create the new memory
    void* pNewMemory;

    WfaResult result = wfaDynamicAllocatorAlloc(dynamicAllocator, size, alignment, freeMemoryFindType, &pNewMemory);
    if(result != WFA_SUCCESS)
        return result;
    
    // Copy from the old memory to the new one
    memcpy(pNewMemory, *ppMemory, (originalSize < size) ? originalSize : size);

    return WFA_SUCCESS;
}
WfaResult wfaDynamicAllocatorFree(WfaDynamicAllocator dynamicAllocator, void* pMemory) {
    if(!dynamicAllocator || !pMemory)
        return WFA_ERROR_INVALID_ARGS;

    // Get the dynamic allocator info
    WfaDynamicAllocatorInfo* info = (WfaDynamicAllocatorInfo*)dynamicAllocator;

    // Get the item's original size and offset
    size_t originalSize = ((size_t*)pMemory)[-1];
    size_t originalOffset = (size_t)pMemory - (size_t)info->memory;

    // Find the closest item after the original memory block
    WfaFreeListItem* currentItem = info->freeListHead,* previousItem = NULL;

    while(currentItem->offset < originalOffset) {
        previousItem = currentItem;
        currentItem = currentItem->pNext;
    }

    // Free the old memory
    if(previousItem && previousItem->offset + previousItem->size == originalOffset) {
        // Set the previous item's new size
        previousItem->size += sizeof(size_t) + originalSize;

        if(currentItem->offset == originalOffset + sizeof(size_t) + originalSize) {
            // Remove the current item from the linked list
            previousItem->pNext = currentItem->pNext;

            // Reset the current item
            previousItem->offset = 0;
            previousItem->size = 0;
            previousItem->pNext = NULL;
        }
    } else if(currentItem->offset == originalOffset + sizeof(size_t) + originalSize) {
        // Set the current item's new offset and size
        currentItem->offset -= sizeof(size_t) + originalSize;
        currentItem->size += sizeof(size_t) + originalSize;
    } else {
        // Find a new item
        WfaFreeListItem* newItem = info->freeListStart;
        WfaFreeListItem* freeListEnd = info->freeListStart + info->freeListMaxSize;

        while(newItem->size && newItem < freeListEnd)
            ++newItem;

        if(newItem == freeListEnd)
            return WFA_ERROR_FREE_LIST_FULL;
        
        // Add the new item to the linked list
        if(previousItem)
            previousItem->pNext = newItem;
        else
            info->freeListHead = newItem;
        
        newItem->pNext = currentItem;
    }

    return WFA_SUCCESS;
}
void wfaDestroyDynamicAllocator(WfaDynamicAllocator dynamicAllocator) {
    free(dynamicAllocator);
}