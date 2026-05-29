#pragma once
#include <stdio.h>
#include <stdbool.h>
#include <vulkan/vulkan.h>
extern VkCommandPool command_pool;
extern VkCommandBuffer* command_buffers;
extern VkDeviceSize destination_offset;
extern VkSemaphore semaphorecmd;
extern VkFence fencecmd;
extern VkCommandBuffer command_buffer;
extern VkSemaphore renderfinishedsemaphore;
bool creatingcommand();
bool renderloop();
void cmdbuffer_cleanup();
bool creatingsemaphore();
bool creatingfence();
bool submitting();
bool destroyingfence();
bool destroyingSemaphore();
bool destroyingcmd();
bool destroyingcmdpool();