#include "WireframeAllocator.hpp"
#include "vk_enum_string_helper.h"
#include <stdio.h>

int main(int argc, char** args) {
    VkApplicationInfo appInfo;

    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = nullptr;
    appInfo.pApplicationName = "Wireframe Allocator Tests";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo;

    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledLayerCount = 0;
    createInfo.ppEnabledLayerNames = nullptr;
    createInfo.enabledExtensionCount = 0;
    createInfo.ppEnabledExtensionNames = nullptr;

    VkInstance instance;

    auto result = vkCreateInstance(&createInfo, wfa::GetAllocationCallbacks(), &instance);
    if(result != VK_SUCCESS) {
        fprintf(stderr, "Failed to create instance! Error code: %s", string_VkResult(result));
        return 1;
    }

    vkDestroyInstance(instance, wfa::GetAllocationCallbacks());

    return 0;
}