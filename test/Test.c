#include "WireframeAllocator.h"
#include <stdio.h>

int main(int argc, char** args) {
    VkApplicationInfo appInfo;

    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = NULL;
    appInfo.pApplicationName = "Wireframe Allocator Tests";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo;

    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pNext = NULL;
    createInfo.flags = 0;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledLayerCount = 0;
    createInfo.ppEnabledLayerNames = 0;
    createInfo.enabledExtensionCount = 0;
    createInfo.ppEnabledExtensionNames = 0;

    VkInstance instance;

    VkResult vkResult = vkCreateInstance(&createInfo, wfaGetVulkanAllocationCallbacks(), &instance);
    if(vkResult != VK_SUCCESS) {
        fprintf(stderr, "Failed to create instance! Error code: %u", (uint32_t)vkResult);
        return 1;
    }

    vkDestroyInstance(instance, wfaGetVulkanAllocationCallbacks());

    WfaDynamicAllocatorCreateInfo daCreateInfo;
    daCreateInfo.size = 65536;
    daCreateInfo.freeListMaxSize = 32;

    WfaDynamicAllocator dynamicAllocator;

    WfaResult result = wfaCreateDynamicAllocator(&daCreateInfo, &dynamicAllocator);

    int* arr;
    result = wfaDynamicAllocatorAlloc(dynamicAllocator, sizeof(int) * 5, 1, WFA_FREE_MEMORY_FIND_FIRST_FIT, (void**)&arr);

    for(int i = 0; i < 5; ++i)
        arr[i] = i;

    result = wfaDynamicAllocatorRealloc(dynamicAllocator, sizeof(int) * 8, 1, WFA_FREE_MEMORY_FIND_FIRST_FIT, (void**)&arr);

    for(int i = 5; i < 8; ++i)
        arr[i] = i;

    for(int i = 0; i < 8; ++i)
        printf("%i; ", arr[i]);
    putchar('\n');

    result = wfaDynamicAllocatorFree(dynamicAllocator, arr);
    wfaDestroyDynamicAllocator(dynamicAllocator);

    return 0;
}