#pragma once 
#include <stdio.h>
#include <stdbool.h>
#include <vulkan/vulkan.h>
bool enumeratingavailablephysicaldevice();
bool loadingdevicelevelfunctions();
bool loadingdevicelevelfunctionsfromextension();
bool loadinglogicaldevice();
extern VkPhysicalDeviceFeatures device_features;
extern VkPhysicalDeviceProperties device_properties;
extern VkQueueFamilyProperties* queue_families;
extern  uint32_t queue_families_count;//SEE THERE ARE GRAPHICS QUEUE COMPUTE QUEUE IT IS JUST GROUP OF QUEUE
extern uint32_t compute_queue_family_index;
extern uint32_t graphics_queue_family_index;
extern VkDevice logical_device;
extern  VkPhysicalDevice physical_device;
extern uint32_t queue_index;
extern VkQueue graphics_queue;
extern VkQueue compute_queue;
void device_cleanup();
bool gettingqueue();
bool destroyinglogicaldevice();

