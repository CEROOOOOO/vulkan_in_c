#pragma once
#include <stdio.h>
#include <vulkan/vulkan.h>
#include <stdbool.h>
extern VkRenderPass render_pass;
extern VkFramebuffer framebuffer;
bool renderpassandframebuffer();
extern VkFramebuffer* swapchain_frame_buffers;
void renderpass_cleanup();