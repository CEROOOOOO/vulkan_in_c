#include <stdio.h>
#include <stdlib.h>
#include <vulkan/vulkan.h>
#include <stdbool.h>
#include "present.h"
#include "device.h"
VkResult resultr = VK_SUCCESS;
VkRenderPass render_pass;
VkFramebuffer framebuffer;
VkFramebuffer* swapchain_frame_buffers = NULL;
bool renderpassandframebuffer()     // idk how this works but think like there are wo swapchain and one semaphore is signaked when image is acqwuired after that image aquiring there are many steps
{                                   // maily all are in command buffer this is alsp in cmd buffer like wheen i call render pass i have to specify different drawing command for that image like i did all these buit at last these all point towarsd render pass
// like color attachembnt it have multiple suppass for multiple things which can be done in render pass
    // at last everyhting i fed to framebuufer no it hold everything and is respoonsible for showinfg
    //  liikethere may be many things liike many images so we are seeting group of images or attachments
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //----------------------------------------------------------------ATTACHMENTS---------------------------------------------------------------------------------------------------------------------//
    //--> lets create a rebder pass with two subpass--first render in two attchments coor and depth -->second subpass reads data from first subpass (ig) and renders into otherattachment
 /*   std::vector<VkAttachmentDescription> attachments_descriptions = {
        {0,
         VK_FORMAT_R8G8B8A8_UNORM,
         VK_SAMPLE_COUNT_1_BIT,
         VK_ATTACHMENT_LOAD_OP_CLEAR,
         VK_ATTACHMENT_STORE_OP_STORE,
         VK_ATTACHMENT_LOAD_OP_DONT_CARE,
         VK_ATTACHMENT_STORE_OP_DONT_CARE,
         VK_IMAGE_LAYOUT_UNDEFINED,
         VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL},
        {0,
         VK_FORMAT_D16_UNORM,
         VK_SAMPLE_COUNT_1_BIT,
         VK_ATTACHMENT_LOAD_OP_CLEAR,
         VK_ATTACHMENT_STORE_OP_STORE,
         VK_ATTACHMENT_LOAD_OP_DONT_CARE,
         VK_ATTACHMENT_STORE_OP_DONT_CARE,
         VK_IMAGE_LAYOUT_UNDEFINED,
         VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL},
        {0,
         VK_FORMAT_R8G8B8A8_UNORM,
         VK_SAMPLE_COUNT_1_BIT,
         VK_ATTACHMENT_LOAD_OP_CLEAR,
         VK_ATTACHMENT_STORE_OP_STORE,
         VK_ATTACHMENT_LOAD_OP_DONT_CARE,
         VK_ATTACHMENT_STORE_OP_DONT_CARE,
         VK_IMAGE_LAYOUT_UNDEFINED,
         VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
        } };*///i dont need all these for a triangle 
    VkAttachmentDescription color_attachment = {};
    color_attachment.format = image_format;
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    //------------------------------------------------------------------------SUBPASS DEPENDENCY----------------------------------------------------------------------------------------------------------//
   /* struct SubpassParameters
    {
        VkPipelineBindPoint PipelineType;                      // defines type of pipeline garphics or compute
        std::vector<VkAttachmentReference> InputAttachments;   // from this we will read data during sub pass
        std::vector<VkAttachmentReference> ColorAttachments;   // all color attchments during subpass
        std::vector<VkAttachmentReference> ResolveAttachments; // changes to be done it amy have its own subpass
        VkAttachmentReference const* DepthStencilAttachment;   // idk
        std::vector<uint32_t> PreserveAttachments;             // like the attachmnst which will be not useful for this set of render pass and subpasses but i may need this in next render pass but idk wjhy to preserve i can create any time
    };
    std::vector<VkSubpassDescription> subpass_descriptions;
    std::vector<SubpassParameters> subpass_parameters;// now our subpass have everything
    VkAttachmentReference depth_stencil_attachment = {
    1,
    VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };
    subpass_parameters = {
        {
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        {},
        {
            {
                0,
                VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            }
        },
        {},
        &depth_stencil_attachment,
        {}

        },//# 1ST SUBPASS
                {
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        {
            {
                0,
                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            }
        },
        {
            {
                2,
                VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            }
        },
        {},
        nullptr,
        {}

        }//#
    };
    for (auto& subpass_description : subpass_parameters) // here we are just copying every element frrom our struct to some struct idk why

    {
        subpass_descriptions.push_back({
            /////holy moly shit our required vector we are pushing back our structs parameter woth the help of other struct like think only one question why why why why
            0,
            subpass_description.PipelineType,
            static_cast<uint32_t>(subpass_description.InputAttachments.size()),
            subpass_description.InputAttachments.data(),
            static_cast<uint32_t>(subpass_description.ColorAttachments.size()),
            subpass_description.ColorAttachments.data(),
            subpass_description.ResolveAttachments.data(),
            subpass_description.DepthStencilAttachment,
            static_cast<uint32_t>(subpass_description.PreserveAttachments.size()),
            subpass_description.PreserveAttachments.data()
            });
    };
    //------------------memory barriers in subpasses like if one subpass want the previous subpass avlue we define subpass dependency---------------------------------------------------------------//
    std::vector<VkSubpassDependency>subpass_dependencies = {
        {
            0,
            1,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            VK_ACCESS_INPUT_ATTACHMENT_READ_BIT,
            VK_DEPENDENCY_BY_REGION_BIT
}
    };// i am sure thesee are important so listen carefully
    //render pass -> just the instructions for images we supply to swapchain
    //subpasses -> like it divides the operations which are supposed to by done by render pass and it will hacw multipkle supass for a render pass
    //like on subpass for color one for depth one for blah blah blah blah
    //these all are combineed and can be called a render pass .....................
    //--------------------------lets fucking create render pass CREATING RENDER PASS------------------------------------------------------------------------------------------------------------------//
    VkRenderPassCreateInfo render_pass_create_info = {
        VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        nullptr,
        0,
        static_cast<uint32_t>(attachments_descriptions.size()),
        attachments_descriptions.data(),
        static_cast<uint32_t>(subpass_descriptions.size()),
        subpass_descriptions.data(),
        static_cast<uint32_t>(subpass_dependencies.size()),
        subpass_dependencies.data()
    };
    resultr = vkCreateRenderPass(logical_device, &render_pass_create_info, nullptr, &render_pass);//now our render pass is ready to be submitted but wait a minutewith render passs there are only few like geometry verrtices it
    if (VK_SUCCESS != resultr)//so we have to create frame buffers which have like information abot the frame we should display they define what images should beused for correswponding attachments defined by our holy moly rener pass
    {
        std::cout << "error creating render pass";
    }*/
    //I AM FOLLOWING VULKAN TUTORIAL CAUSE ITS FOCUSED ON A TRIANGLE NOT EVERY FUNCTION
    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    VkRenderPassCreateInfo render_pass_create_info = {};
    render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_create_info.attachmentCount = 1;
    render_pass_create_info.pAttachments = &color_attachment;
    render_pass_create_info.subpassCount = 1;
    render_pass_create_info.pSubpasses = &subpass;
    resultr = vkCreateRenderPass(logical_device, &render_pass_create_info, NULL, &render_pass);
    if (resultr != VK_SUCCESS) {
        printf("error creating render pass\n");
        return false;
    }
    /////-------------------------------------------------------------CREATING FRAMEBUFFER-------------------------------------------------------------------------------------------------------------------------------////////
   /* std::vector<VkImageView>attachments;
    VkFramebufferCreateInfo frame_buffer_create_info = {
        VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
    nullptr,
    0,
    render_pass,
    static_cast<uint32_t>(attachments.size()),
    nullptr,
    600,
    600,
    0
    };
    resultr = vkCreateFramebuffer(logical_device, &frame_buffer_create_info, nullptr, &framebuffer);
    if (resultr != VK_SUCCESS) {
        std::cout << "error creating frame buffer\n";
        return false;
    }
    //___________________________________________craeting something render_______________________________________________//
    return true;
    */
    //I AM FOLLOWING A VULKAN TUTORIAL THOOOOOO I THINNK THIS BOOK TELLS WAY MORE THEN NEEDED I LOVED BOOK BOOK IS NOT THE PROBLEM I AM THE PROBLEM \

    //LIKE SEE FARMEBUFFERS MEANS JUST THE AREA IN WINDOW I WILL DRAW THINGS ON I DEFINED A RENDER PASS WITH SOME CRUCIAL INSTRUCTIONS THOSE INSTRUCTIONS NEED TO BE 
    //PLACED SOMEWHERE SO FRAMEBUUFERS IS THE POINT WHERE ALL THESE FUNCTIONS I DEFINED ABOVE WILL RENDER 
    //I HAVE 3 SLIDES OFSWAPCHAIN IMAGES SO I NEED 3 FRAMEBUFFERS RATHER THAN HARD CODING JUST CREATE A VECTOR
    swapchain_frame_buffers = (VkFramebuffer*)malloc(images_count * sizeof(VkFramebuffer));
    for (size_t i = 0; i < images_count; i++) {
        VkImageView attachments[] = {
            swapchain_image_views[i]
        };
        VkFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = render_pass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = size_of_images.width;
        framebufferInfo.height = size_of_images.height;
        framebufferInfo.layers = 1;
        resultr = vkCreateFramebuffer(logical_device, &framebufferInfo, NULL, &swapchain_frame_buffers[i]);
        if (resultr != VK_SUCCESS) {
            printf("error creating frame buffer\n");
            return false;
        }
    }
    framebuffer = swapchain_frame_buffers[image_index];//image index is the index of next available image which is defined by vkacquire nexrt image khr
    return true;
}
void renderpass_cleanup()
{
    free(swapchain_frame_buffers);
    vkDestroyRenderPass(logical_device, render_pass, NULL);
    for(int i = 0; i < images_count; i++) {
        vkDestroyFramebuffer(logical_device, swapchain_frame_buffers[i], NULL);
    }
    vkDestroyFramebuffer(logical_device, framebuffer, NULL);
}