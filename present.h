#pragma once
#include <stdio.h>
#include <vulkan/vulkan.h>
#include <stdbool.h>
#include <GLFW/glfw3.h>
bool presentingsurface();
extern VkSurfaceKHR presentation_surface;
bool selectingqueuefamilythatsupportspresentationforgivensurface();
extern VkPresentModeKHR present_mode;
extern VkSurfaceCapabilitiesKHR surface_capabilities;
bool selectingdesiredpresentationmode();
bool capabilitiesofpresentationsurface();
bool selectingnumberofswapchainimages();
bool choosingsizeofswapchainimages();
bool desiredusageofswapchainimages();
bool transformationofswapchainimages();
bool formatofswapchainimages();
bool createswapchain();
bool gettinghandlesofswapchain();
bool creatingswapchainimageviews();
bool creatingsemaphoreimg();
bool creatingfenceimg();
void present_cleanup();
bool acquiringswapchainimages();
bool presentinganimage();
extern uint32_t number_of_images;
extern VkExtent2D size_of_images;
extern VkSurfaceTransformFlagBitsKHR surface_transform;
extern VkFormat image_format;
extern VkColorSpaceKHR image_color_space;
extern VkSurfaceFormatKHR* surface_formats;
extern VkSwapchainKHR swapchain;
extern VkSemaphore imageavailablesemaphore;//signaled in vkacquirenext
extern VkFence inflightfence;//so this waits indefinetly full untill the queue submission
extern VkQueue present_queue;
extern VkImage* swapchain_images;
extern GLFWwindow* window;
extern uint32_t image_index;
extern VkImageView* swapchain_image_views;
extern uint32_t images_count;