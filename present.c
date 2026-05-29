#include <stdio.h>
#include <stdlib.h>
#include <vulkan/vulkan.h>
#include "instance.h"
#include "device.h"
#include "present.h"
#include "buffer.h"
#include "cmdbuffer.h"
#include <GLFW/glfw3.h>
VkResult resultp = VK_SUCCESS;
VkResult resultp1 = VK_SUCCESS;
VkSurfaceKHR presentation_surface = VK_NULL_HANDLE;
VkPresentModeKHR present_mode;
VkSurfaceCapabilitiesKHR surface_capabilities;
uint32_t number_of_images;
VkExtent2D size_of_images;
VkSurfaceTransformFlagBitsKHR surface_transform;
VkFormat image_format = VK_FORMAT_R8G8B8A8_UNORM;
VkColorSpaceKHR image_color_space = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
VkSurfaceFormatKHR* surface_formats = NULL;
VkSwapchainKHR swapchain;
VkSemaphore imageavailablesemaphore;//signaled in vkacquirenext
VkFence inflightfence;//so this waits indefinetly full untill the queue submission
VkQueue present_queue;
uint32_t present_queue_index;
VkImage* swapchain_images = NULL;//we are using vector for swapchain images because we can resize it later on
uint32_t image_index;
VkImageView* swapchain_image_views = NULL;//we are using vector for swapchain image views because we can resize it later on
VkPresentModeKHR* present_modes = NULL;
VkFence* inflightfences = NULL;
VkSwapchainKHR* swapchains = NULL;
VkSemaphore* waitsignalsemaphores = NULL;
uint32_t* image_indices = NULL;
GLFWwindow* window;
uint32_t images_count = 0;
bool presentingsurface()
{//nothing i a just creating window surface
    if (!glfwInit()) {
        return -1;
    }
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(1280, 720, "Vulkan Window", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    resultp = glfwCreateWindowSurface(instance, window, NULL, &presentation_surface);

    if ((VK_SUCCESS != resultp) || (VK_NULL_HANDLE == presentation_surface))
    {
        printf("couldnt create presentation surface\n");
         glfwDestroyWindow(window);
        glfwTerminate();    
        return false;
    }
    return true;
}
bool selectingqueuefamilythatsupportspresentationforgivensurface()
{

    uint32_t present_queue_family_index = 0;
    for (uint32_t INDEX = 0; INDEX < queue_families_count; INDEX++)
    {
        VkBool32 presentation_supported = VK_FALSE;
         resultp = vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, INDEX, presentation_surface, &presentation_supported);
        if ((VK_SUCCESS == resultp) && (VK_TRUE == presentation_supported))
        {
            present_queue_family_index = INDEX;
            printf("present queue family index is %u\n", present_queue_family_index);
            break;//just getting one queue cause why to go check if i only want one
        }
    }
    vkGetDeviceQueue(logical_device, present_queue_family_index, queue_index, &present_queue);
    return true;
}
bool selectingdesiredpresentationmode()
{
    VkPresentModeKHR desired_present_mode = VK_PRESENT_MODE_MAILBOX_KHR;//so its like my desired presentation mode of swapchain
    uint32_t present_modes_count = 0;
    resultp = vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, presentation_surface, &present_modes_count, NULL);//getting modes count first 
    if ((resultp != VK_SUCCESS) || present_modes_count == 0)
    {
        printf("cant get present modes count\n");
        return false;
    }
    present_modes = malloc(present_modes_count * sizeof(VkPresentModeKHR));
    resultp = vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, presentation_surface, &present_modes_count, present_modes);
    if ((resultp != VK_SUCCESS) || (present_modes_count == 0))
    {
        printf("cant get present modes storage\n");
        return false;
    }
    for (uint32_t i = 0; i < present_modes_count; i++)//put present modes in current_present_mode
    {
        if (present_modes[i] == desired_present_mode)
        {
            present_mode = desired_present_mode;//put it in present mode if our desired mode is there then put it here 
            return true;
        }
    }
    for (uint32_t i = 0; i < present_modes_count; i++)
    {
        if (present_modes[i] == VK_PRESENT_MODE_FIFO_KHR)//if fifo is there pu t it in present mode cause its only mode which is guaranteed to be there so if our desired mode is not there then put this one
        {
            present_mode = VK_PRESENT_MODE_FIFO_KHR;
            return true;
        }
    }
    return true;

}
bool capabilitiesofpresentationsurface()
{
    resultp = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, presentation_surface, &surface_capabilities);
    if (resultp != VK_SUCCESS)
    {
        printf("could not get the capabilities of presentation surface\n");
        return false;
    }                            

    return true;
}
bool selectingnumberofswapchainimages()
{
    number_of_images = surface_capabilities.minImageCount + 1; // in some case if max is unfortunately zero then jusrt give number_of_images some value so it gets it even if max image is zero
    if ((surface_capabilities.maxImageCount > 0) && (number_of_images > surface_capabilities.maxImageCount))
    { // check is max is > 0 ( which is always true) but some fucking device have max image 2 ansd min image also 2 so when min image +1 get greator than 0 we can check if max image is 2 or not if 2 then number_of_images gets greater than max

        number_of_images = surface_capabilities.maxImageCount; // here its saying shut the fuck up  number_of_images you are instructed and should obey maximage count of yop
    }
    return true;
}
bool choosingsizeofswapchainimages()
{
    if (surface_capabilities.currentExtent.width == 0xFFFFFFFF)
    {                                // this 0xFFFFFFFF means some os chose window size based on swapchain images so check if our os is also that fool
        size_of_images.height = 720; // then set the size of image
        size_of_images.width = 1280;
        //downside code will handle even if you gave wrong dont worry 
        if (size_of_images.width < surface_capabilities.minImageExtent.width)
        {                                                                     // if our defined size of image is less than os's size
            size_of_images.width = surface_capabilities.minImageExtent.width; // assign it surface capabilities just give os's size dont interfere
        }
        else if (size_of_images.width > surface_capabilities.maxImageExtent.width)
        {
            size_of_images.width = surface_capabilities.maxImageExtent.width;
        }
        if (size_of_images.height < surface_capabilities.minImageExtent.height)
        {                                                                       // if our defined size of image is less than os's size
            size_of_images.height = surface_capabilities.minImageExtent.height; // assign it surface capabilities
        }
        else if (size_of_images.height > surface_capabilities.maxImageExtent.height)
        {
            size_of_images.height = surface_capabilities.maxImageExtent.height;
        }
    }
    else
    {
        size_of_images = surface_capabilities.currentExtent;
    }
    return true;
}
bool desiredusageofswapchainimages()
{
    VkImageUsageFlags desired_usages = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;//just think liike it can be added colors to it in framebuffer
    VkImageUsageFlags image_usage = 0;
    image_usage = desired_usages & surface_capabilities.supportedUsageFlags;//seee that my window uses it or not 
    return desired_usages == image_usage;
    return true;
}
bool transformationofswapchainimages()
{
    VkSurfaceTransformFlagBitsKHR desired_transform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;//just means no transformastion in swapchain images is required 
    if (surface_capabilities.supportedTransforms & desired_transform)
    {
        surface_transform = desired_transform;//if supported transforms has our desried transform then put surface transform as desired transform
    }
    else
    {
        surface_transform = surface_capabilities.currentTransform;
    }
    return true;
}
bool formatofswapchainimages()
{ // like what and how and which color to use which data type
    VkSurfaceFormatKHR desired_surface_format = {
        image_format,
        image_color_space };
    uint32_t formats_count = 0;
    resultp = vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, presentation_surface, &formats_count, NULL);
    if ((resultp != VK_SUCCESS) || (formats_count == 0))
    {
        printf("error no formats are there\n");
        return false;
    }
    surface_formats = malloc(sizeof(VkSurfaceFormatKHR)*formats_count);
    resultp = vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, presentation_surface, &formats_count, &surface_formats[0]);
    if ((resultp != VK_SUCCESS) || (formats_count == 0))
    {
        printf("couldnt enumerate surface formats\n");
        return false;
    }
    if ((formats_count == 1) && (surface_formats[0].format == VK_FORMAT_UNDEFINED)) // check if our surface formats has undefined then btw surface format can never be zero is zero then you are wrong in other thing
    {
        image_format = desired_surface_format.format;          // give image format our desired format
        image_color_space = desired_surface_format.colorSpace; // give color space our desired color space
        return true;
    }
    for (uint32_t i = 0; i < formats_count; i++)
    { // store our surfaceformats in suurface format
        if ((desired_surface_format.format == surface_formats[i].format) && (desired_surface_format.colorSpace == surface_formats[i].colorSpace))
        {                                                          // if our desired values is equal to the surface values
            image_format = desired_surface_format.format;          // assign desired to image format
            image_color_space = desired_surface_format.colorSpace; // assign desired to color space
            return true;
        }
    }
    for (uint32_t i = 0; i < formats_count; i++)
    {
        if ((desired_surface_format.format == surface_formats[i].format))
        {                                                  // check if our desired surface format is equal to the surface format which is given by the os
            image_format = desired_surface_format.format;  // assign desired to image format
            image_color_space = surface_formats[i].colorSpace; // just give the color space of our surface format like if we are sure image format is provided but color space is not formatted
            printf("desired combination of format is supported but no colorspace ");
            return true;
        }
    }
    image_format = surface_formats[0].format;
    image_color_space = surface_formats[0].colorSpace;
    printf("desired format is not supported but we are taking the first format which is %d and color space is %d\n", image_format, image_color_space);
    return true;
}

bool createswapchain()/////its zero btw thats why i am getting error
{
    VkExtent2D image_size = size_of_images;
    VkImageUsageFlags image_usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // idk whats it but book says desired usage
    VkSwapchainKHR old_swapchain = VK_NULL_HANDLE;                       // if there is a swapchain created previously store a handle to it or VK_NULL_HANDLE
    VkSwapchainCreateInfoKHR swapchain_create_info = {
        VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        NULL,
        0,
        presentation_surface,
        number_of_images,
        image_format,
        image_color_space,
        image_size,
        1,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        VK_SHARING_MODE_EXCLUSIVE,
        0,
        NULL,
        surface_transform,
        VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        present_mode,
        VK_TRUE,
        old_swapchain };
    resultp = vkCreateSwapchainKHR(logical_device, &swapchain_create_info, NULL, &swapchain);
    if ((resultp != VK_SUCCESS) || (swapchain == VK_NULL_HANDLE))
    {
        printf("couldnt create a swapchain\n"); 
            printf("swapchain%p\n", (void*)swapchain);
        return false;
    }
    if (old_swapchain != VK_NULL_HANDLE) // if something is in old_swapchain that happens when new image gets added to queue
    {
        vkDestroySwapchainKHR(logical_device, old_swapchain, NULL);
        old_swapchain = VK_NULL_HANDLE;
    }

    return true;
}
bool gettinghandlesofswapchain() // safe all from her
{
    resultp = vkGetSwapchainImagesKHR(logical_device, swapchain, &images_count, NULL);//////////////////////////////this is whee i am getting error
    if ((resultp != VK_SUCCESS) || (images_count == 0))
    {
        printf("cant get swapchain images count\n");
        return false;
    } // dont look confused i made swapchain_images global vector idk why
    swapchain_images = malloc(sizeof(VkImage)*images_count);// we are making space for swapchain images based on count of images
    resultp = vkGetSwapchainImagesKHR(logical_device, swapchain, &images_count, &swapchain_images[0]); // like now we are getting handles of swapchain image
    if ((resultp != VK_SUCCESS) || (images_count == 0))
    {
        printf("cant get swapchain images\n");
        return false;
    }
    return true;
}
bool creatingswapchainimageviews() {

    swapchain_image_views = malloc(sizeof(VkImageView)*images_count);
    for (size_t i = 0; i < images_count; i++) {
        VkImageViewCreateInfo image_create_info = {};
        image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        image_create_info.image = swapchain_images[i];
        image_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        image_create_info.format = image_format;
        image_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        image_create_info.subresourceRange.baseMipLevel = 0;
        image_create_info.subresourceRange.levelCount = 1;
        image_create_info.subresourceRange.baseArrayLayer = 0;
        image_create_info.subresourceRange.layerCount = 1;
        resultp = vkCreateImageView(logical_device, &image_create_info, NULL, &swapchain_image_views[i]);
        if (resultp != VK_SUCCESS) {
            printf(" cant get image views \n");
            return false;
        }
    }
    return true;
}
bool creatingsemaphoreimg()
{
    VkSemaphoreCreateInfo swapchain_semaphore_create_info = {
        VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        NULL,
        0 };
    resultp = vkCreateSemaphore(logical_device, &swapchain_semaphore_create_info,     NULL, &imageavailablesemaphore);
    resultp1 = vkCreateSemaphore(logical_device, &swapchain_semaphore_create_info, NULL, &renderfinishedsemaphore);//ok render finished semaphore wait for the render to finish and then it gets signaled and then we can present the image
    if (VK_SUCCESS != resultp)
    {
        printf("error creating semaphores\n");
        return false;
    }
    if (resultp1 != VK_SUCCESS)
    {
        printf("error creating render finished semaphore\n");
        return false;
    }
    return true;
}
bool creatingfenceimg()                // same like semaphore but here the commands after getting executed from queue to gpu if we want the cpu to know that this command is executed we can use fence
{                                   // it also have sign aled and unsignaled condition example we have cmd buffer it executes in transfer queue and along with it fence is attached if we take a screenshot
    // now untill the fence is signaled which will happen after that queue is executed untill then cpu waits idle so we after have screeenshot with correct sequqence
    VkFenceCreateInfo swapchain_fence_create_info = {};
    swapchain_fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    swapchain_fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    swapchain_fence_create_info.pNext = 0;
    resultp = vkCreateFence(logical_device, &swapchain_fence_create_info, NULL, &inflightfence);
    if (VK_SUCCESS != resultp)
    {
        printf("could'nt create fence\n");
        return false;
    }

    return true;
}
bool acquiringswapchainimages()
{
    // dont look confused semaphore fence and image index i made them global variables so others can use it
    //in loop i am supposed to do alll these but wait untill the rendering is done like this
    inflightfences = malloc(sizeof(VkFence)*1);
    inflightfences[0] = inflightfence;
    vkWaitForFences(logical_device, 1, &inflightfences[0], VK_TRUE, UINT64_MAX);
    vkDeviceWaitIdle(logical_device);
    vkResetFences(logical_device, 1, &inflightfences[0]);
    resultp = vkAcquireNextImageKHR(logical_device, swapchain, 2000000000, imageavailablesemaphore, VK_NULL_HANDLE, &image_index); // vkacquirenextimagekhr means it gives index of image which is free means when the presentation is over then it gives that index to repeat the swapchain and we givw it a image available semaphore
   //so imageavailablesemaphore is signaled from here like next image is ready 
    switch (resultp)
    {
    case VK_SUCCESS:
    case VK_SUBOPTIMAL_KHR:
        return true;
    default:
        return false;
    }
    return true;
}
bool presentinganimage()
{
    swapchains = malloc(sizeof(VkSwapchainKHR)*1);
    swapchains[0] = swapchain;
    waitsignalsemaphores = malloc(sizeof(VkSemaphore)*1);
    waitsignalsemaphores[0] = renderfinishedsemaphore;//i will wait for render finished semaphore tho which i will get from vkqueuesubmit i guess
    image_indices = malloc(sizeof(uint32_t)*1);
    image_indices[0] = image_index;
    VkPresentInfoKHR present_info = {
        VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        NULL,
        1,//its waiting on semaphore by which is signaled after vkqueue submit
        &waitsignalsemaphores[0],
        1,
        &swapchains[0],
        &image_indices[0],
        NULL
    };
    resultp = vkQueuePresentKHR(present_queue, &present_info);
    switch (resultp)
    {
    case VK_SUCCESS:
        return true;
    default:
        return false;
    }
    
}
void present_cleanup()
{
    free(swapchain_images);
    free(swapchain_image_views);
    free(present_modes);
    free(inflightfences);
    free(swapchains);
    free(waitsignalsemaphores);
    free(image_indices);
    free(surface_formats);
    vkDestroySemaphore(logical_device, imageavailablesemaphore, NULL);
    vkDestroySemaphore(logical_device, renderfinishedsemaphore, NULL);
    vkDestroyFence(logical_device, inflightfence, NULL);
    vkDestroySwapchainKHR(logical_device, swapchain, NULL);
    vkDestroySurfaceKHR(instance, presentation_surface, NULL);
}