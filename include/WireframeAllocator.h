#pragma once
#include "vulkan.h"

#ifdef __cplusplus
extern "C" {
#endif

/// @brief Returns a pointer to the Vulkan allocation callbacks.
VkAllocationCallbacks* wfaGetAllocationCallbacks();
/// @brief Returns a pointer to the Vulkan host allocation function.
PFN_vkAllocationFunction wfaGetAllocationFunctionPtr();
/// @brief Returns a pointer to the Vulkan host reallocation function.
PFN_vkReallocationFunction wfaGetReallocationFunctionPtr();
/// @brief Returns a pointer to the Vulkan host free function.
PFN_vkFreeFunction wfaGetFreeFunctionPtr();
/// @brief Returns a pointer to the Vulkan internal allocation notification.
PFN_vkInternalAllocationNotification wfaGetInternalAllocationNotificationPtr();
/// @brief Returns a po9inter to the Vulkan internal free notification.
PFN_vkInternalFreeNotification wfaGetInternalFreeNotificationPtr();

#ifdef __cplusplus
}
#endif