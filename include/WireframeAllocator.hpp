#pragma once
#include "vulkan.hpp"

namespace wfa {
    /// @brief Returns a pointer to the Vulkan allocation callbacks.
    VkAllocationCallbacks* GetAllocationCallbacks();
    /// @brief Returns a pointer to the Vulkan host allocation function.
    PFN_vkAllocationFunction GetAllocationFunctionPtr();
    /// @brief Returns a pointer to the Vulkan host reallocation function.
    PFN_vkReallocationFunction GetReallocationFunctionPtr();
    /// @brief Returns a pointer to the Vulkan host free function.
    PFN_vkFreeFunction GetFreeFunctionPtr();
    /// @brief Returns a pointer to the Vulkan internal allocation notification.
    PFN_vkInternalAllocationNotification GetInternalAllocationNotificationPtr();
    /// @brief Returns a po9inter to the Vulkan internal free notification.
    PFN_vkInternalFreeNotification GetInternalFreeNotificationPtr();
}