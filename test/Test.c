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

    VkResult result = vkCreateInstance(&createInfo, wfaGetAllocationCallbacks(), &instance);
    if(result != VK_SUCCESS) {
        fprintf(stderr, "Failed to create instance! Error code: %u", (uint32_t)result);
        return 1;
    }

    vkDestroyInstance(instance, wfaGetAllocationCallbacks());

    return 0;
}