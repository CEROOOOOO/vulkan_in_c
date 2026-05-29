#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include "instance.h"
#include "vector.h"
#include <string.h>
VkInstance instance;//SEE HERE I AM DECLARING INSTANCE WHICH WAS IN EXTERN BEFORE SO NOW THIS INSTANCE HAVE THE DATA ]
//IF I INCLUDED HEADER INSTANCE THEN THE INSTANCE WILL BE DECLARED SO DONT DECLARE NOW IF YOU DECLARED HERE 
const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;
VkLayerProperties* availablelayers = NULL;

const char* validationlayers[] = {"VK_LAYER_KHRONOS_validation"};
bool checkvalidationlayersupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, NULL);
    availablelayers = malloc(sizeof(VkLayerProperties)*layerCount)  ;
    VkResult result = VK_SUCCESS;
    result = vkEnumerateInstanceLayerProperties(&layerCount, availablelayers);
        for (uint32_t i = 0; i < layerCount; i++) {
        if (strcmp(availablelayers[i].layerName, "VK_LAYER_KHRONOS_validation") == 0)
            return true;
    }
        printf("validation layer not found\n");
    return false;
}
//bool extensioncount()


bool createinstance()
{
     uint32_t extensionnumber = 0 ;
    const char **extensions = glfwGetRequiredInstanceExtensions(&extensionnumber);

    const char* application_name = "VUUUIULKANN";
    VkApplicationInfo application_info = {
        VK_STRUCTURE_TYPE_APPLICATION_INFO,
        NULL,
        application_name,
        VK_MAKE_VERSION(1, 0, 0),
        "vulkan cookbook",
        VK_MAKE_VERSION(1, 0, 0),
        VK_MAKE_VERSION(1, 0, 0) };
    VkInstanceCreateInfo instance_create_info = {
        VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        NULL,
        0,
        &application_info,
        1,
        validationlayers,
        extensionnumber,
        extensions
    };
    VkResult result = vkCreateInstance(&instance_create_info, NULL, &instance);//this function is from vullkan dll so its loading
    if ((result != VK_SUCCESS) || (instance == VK_NULL_HANDLE))
    {
        printf("you didnt created instance yet ");
        return false;
    }
    //fuck you 2 is not running woohoh 
    else
    {
        printf("instance created\n");
    }
    return true;
}

bool destroyingvulkaninstance()
{
    if (instance)
    {
        vkDestroyInstance(instance, NULL);
        instance = VK_NULL_HANDLE;
    }
    return true;
}
void instance_cleanup()
{
    free(availablelayers);
    vkDestroyInstance(instance, NULL);
}

