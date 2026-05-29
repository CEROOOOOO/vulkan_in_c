#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <vulkan/vulkan.h>
#include "instance.h"
    typedef struct 
    {
        uint32_t FamilyIndex;
        float* Priorities;
    }QueueInfo;

VkPhysicalDeviceFeatures device_features;//NEVER FORGET TO REDECLARE IN CPP WHICH WILL USE EXTERN 
VkPhysicalDeviceProperties device_properties;//THAT EXTERN JUST SAID THAT ITS PRESENT BUT NEVER DECLARED
VkQueueFamilyProperties* queue_families = NULL;//FOR QUEUE FAMILIES WE ARE USING VECTOR SO THAT WE CAN RESIZE IT LATER ON
VkPhysicalDevice* available_devices = NULL;
VkExtensionProperties* available_extensions = NULL;
QueueInfo* requested_queues = NULL;
uint32_t compute_queue_family_index;//IF YOU WANT TO USE THAT EXTERN SOMEWHERE ELSE THEN DECLARE THAT STRCUT OR VARIABLE THERE
uint32_t graphics_queue_family_index;//IF DIDNT YOU GET SOME LKNK 2001
VkDevice logical_device;
uint32_t queue_index = 0;
VkQueue graphics_queue;
VkQueue compute_queue;
VkPhysicalDevice physical_device;
 uint32_t queue_families_count = 0;//SEE THERE ARE GRAPHICS QUEUE COMPUTE QUEUE IT IS JUST GROUP OF QUEUE


char const* device_extensions[]= 
{
    "VK_KHR_swapchain"
};
VkResult resultd = VK_SUCCESS;
bool enumeratingavailablephysicaldevice()
{
    uint32_t devices_count = 0;
    resultd = vkEnumeratePhysicalDevices(instance, &devices_count, NULL);///i am storing in devuce count btw
    if ((resultd != VK_SUCCESS) || (devices_count == 0))
    {
        return false;
    }
    available_devices = malloc(sizeof(VkPhysicalDevice)*devices_count);
    resultd = vkEnumeratePhysicalDevices(instance, &devices_count, &available_devices[0]);
    if ((resultd != VK_SUCCESS) || (devices_count == 0))
    {
        printf("you didnt enumerate physical device yet ");
        return false;
    }
    printf("physical device created\n");
    physical_device = available_devices[1];///////////////////nvidia aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa//HERE I AM ASSIGNINNG MY PHYSICAL DEVICE PHYSICAL DEVICE IS CREATED 
    printf("physical_device is %p\n", (void*)physical_device);
    uint32_t extension_count = 0;
    resultd = vkEnumerateDeviceExtensionProperties(physical_device, NULL, &extension_count, NULL);//CHECKING THE EXTENSIONS IT WILL PROVID E
    if ((resultd != VK_SUCCESS) || (extension_count == 0))
    {
        printf("couldnt get extension count");
        return false;
    }
    else
    {
        printf("extension count %u\n", extension_count);
    }
    available_extensions = malloc(sizeof(VkExtensionProperties) * extension_count);
    resultd = vkEnumerateDeviceExtensionProperties(physical_device, NULL, &extension_count, available_extensions);
    if ((resultd != VK_SUCCESS) || (extension_count == 0))
    {
        printf("couldnt enumerate device extensions");
        return false;
    }
    vkGetPhysicalDeviceFeatures(physical_device, &device_features);//LIKE FEATURES AND PROPERTIES
    vkGetPhysicalDeviceProperties(physical_device, &device_properties);
    printf("your device is %s\n", device_properties.deviceName);
    printf("your driver version is %u\n", device_properties.driverVersion)  ;
    if (!device_features.geometryShader)
    {
        printf("geometry shader is not supported you fool\n");
    } 
    else
    {
        device_features.geometryShader = VK_TRUE;
        printf("geometry shader is suppported \n");
    }
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_families_count, NULL);//FOR CREATING ANYTHING WE ARE SUPPOSED TO FORM A QUEUE THERE ARE GRAPHICS QUEUE COMPUTE QUEUE
    if (queue_families_count == 0)
    {
        printf("error getting queue count \n");
        return false;
    }
    else
    {
        printf("queue families count %u\n", queue_families_count);
    }
    queue_families = malloc(sizeof(VkQueueFamilyProperties) * queue_families_count);//RESIZE THE VECTOR TO FIT THE QUEUE FAMILIES COUNT
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_families_count, queue_families);
    if (queue_families_count == 0)
    {
        printf("cant get the queue families itself\n");
        return false;
    }
    VkQueueFlags desired_capabilities_for_graphics = VK_QUEUE_GRAPHICS_BIT;
    for (uint32_t INDEX = 0; INDEX < queue_families_count; INDEX++)
    {
        if ((queue_families[INDEX].queueCount > 0) && (queue_families[INDEX].queueFlags & desired_capabilities_for_graphics))//HERE ITS TO CHECK IF OUR QUEUE FAMILY SUPPORTS GRAPHICS OR NOT 
        {//LISTEN THERE MAY BE MANY FLAGS A QUEUE FAMILY CAN SUPPORT SO THAT & IS CHECKING WHETHER ITS SUPPORTED OR NOT
            graphics_queue_family_index = INDEX;//OK SO IT WILL STORE WHICH QUEUE FAMILY SUPPORTS GHRAPHICS
            printf("graphics queue family index is %u\n", graphics_queue_family_index);
            break;//after finding just break
        }
    }
    VkQueueFlags desired_capabilities_for_compute = VK_QUEUE_COMPUTE_BIT;
    for (uint32_t INDEX = 0; INDEX < queue_families_count; INDEX++)
    {
        if ((queue_families[INDEX].queueCount > 0) && (queue_families[INDEX].queueFlags & desired_capabilities_for_compute))
        {
            compute_queue_family_index = INDEX;//after finding which queue family supports just break;
            printf("compute queue family index is %u\n", compute_queue_family_index);
            break;
        }
    }
    return true;
}
bool loadinglogicaldevice()
{

    requested_queues = malloc(sizeof(QueueInfo)*2);//we are making space for 2 queue cause we want to use graphics and compute queue
    requested_queues[0] = (QueueInfo){graphics_queue_family_index, (float[]){1.0f}};
    if (graphics_queue_family_index != compute_queue_family_index)
    {
        requested_queues[1] = (QueueInfo){compute_queue_family_index, (float[]){1.0f}};
    }
    VkDeviceQueueCreateInfo queue_create_infos; 
    queue_create_infos.flags = 0;
    queue_create_infos.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_infos.pNext = NULL;
    queue_create_infos.queueCount = 1;
    queue_create_infos.pQueuePriorities = requested_queues[0].Priorities;
    queue_create_infos.queueFamilyIndex = requested_queues[0].FamilyIndex; 
    VkDeviceCreateInfo device_create_info = {
        VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        NULL,
        0,
        1,
        &queue_create_infos,
        0,
        NULL,
        1,
        device_extensions,
        &device_features

    };
    resultd = vkCreateDevice(physical_device, &device_create_info, NULL, &logical_device);
    if ((resultd != VK_SUCCESS) || (logical_device == VK_NULL_HANDLE))
    {
        printf("could not create logical device \n");
        return false;
    }
    printf("created logical device\n");
    printf("your logical device is %p\n", (void*)logical_device);
    return true;
}
bool gettingqueue()
{
    vkGetDeviceQueue(logical_device, graphics_queue_family_index, queue_index, &graphics_queue);//so here i am getting graphics queue cause i am entering index of queue family which supported graophics
    vkGetDeviceQueue(logical_device, compute_queue_family_index, queue_index, &compute_queue);
    printf("your graphics queue is %p\n", (void*)graphics_queue);
    printf("your compute queue is %p\n", (void*)compute_queue);
    return true;
}
bool destroyinglogicaldevice()
{
    if (logical_device)
    {
        vkDestroyDevice(logical_device, NULL);
        logical_device = VK_NULL_HANDLE;
    }
    return true;
}
void device_cleanup()
{
    free(queue_families);
    free(requested_queues);
    free(available_devices);
    free(available_extensions);
    vkDestroyDevice(logical_device, NULL);
    
}