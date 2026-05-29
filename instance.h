#pragma once
#include <stdio.h>
#include <vulkan/vulkan.h>
#include <stdbool.h>
bool checkvalidationlayersupport();
bool createinstance();
extern VkInstance instance;
bool destroyingvulkaninstance();
void instance_cleanup();