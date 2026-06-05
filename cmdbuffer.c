#include <stdio.h>
#include <stdlib.h>
#include <vulkan/vulkan.h>
#include "present.h"
#include "device.h"
#include "buffer.h"
#include "renderpass.h"
#include "pipeline.h"
VkDeviceSize destination_offset;
VkResult resultcmd = VK_SUCCESS;
VkCommandPool command_pool;
VkSemaphore renderfinishedsemaphore;
VkCommandBuffer command_buffer;
VkCommandBuffer staging_command_buffer;
VkCommandBuffer *command_buffers = NULL;
VkCommandBuffer *staging_command_buffers = NULL;
VkCommandBufferUsageFlags usage = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
VkCommandBufferInheritanceInfo *secondary_command_buffer_info = NULL;
VkSemaphore *waitsemaphores = NULL;
/// main thing to say call your buffer or renderpass or anything here dont create them  here //////////////////////////////////////
bool creatingcommand()
{
    uint32_t count = images_count;
    VkCommandPoolCreateFlags parameters = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    VkCommandPoolCreateInfo command_pool_create_info = {
        VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        NULL,
        parameters,
        graphics_queue_family_index

    };
    resultcmd = vkCreateCommandPool(logical_device, &command_pool_create_info, NULL, &command_pool);
    if (VK_SUCCESS != resultcmd)
    {
        printf("Couldn't create command pool\n");
        return false;
    }

    VkCommandBufferAllocateInfo command_buffer_allocate_info = {// allocating command buffer
                                                                VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                                                                NULL,
                                                                command_pool,
                                                                VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                                                                count};
    VkCommandBufferAllocateInfo staaging_command_buffer_allocate_info = {
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        NULL,
        command_pool,
        VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        count};
    command_buffers = malloc(count * sizeof(VkCommandBuffer));
    staging_command_buffers = malloc(count * sizeof(VkCommandBuffer));
    resultcmd = vkAllocateCommandBuffers(logical_device, &command_buffer_allocate_info, &command_buffers[0]);
    if (VK_SUCCESS != resultcmd)
    {
        printf("couldn't create command buffer\n");
        return false;
    }
    resultcmd = vkAllocateCommandBuffers(logical_device, &command_buffer_allocate_info, &staging_command_buffers[0]);
    if (VK_SUCCESS != resultcmd)
    {
        printf("couldn't create staging command buffer\n");
        return false;
    }
    staging_command_buffer = staging_command_buffers[0]; // for staging
    VkCommandBufferResetFlags release_resources = VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT;
    resultcmd = vkResetCommandBuffer(staging_command_buffer, release_resources ? VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT : 0);
    if (VK_SUCCESS != resultcmd)
    {
        printf("could'nt RESET command buffer recording operation\n");
        return false;
    }
    VkCommandBufferUsageFlags usage_staging = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    VkCommandBufferInheritanceInfo *staging_secondary_command_buffer_info = NULL;
    VkCommandBufferBeginInfo staging_command_buffer_begin_info = {
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        NULL,
        usage_staging,
        staging_secondary_command_buffer_info};
    resultcmd = vkBeginCommandBuffer(staging_command_buffer, &staging_command_buffer_begin_info); // begined this operation from here i will stage copy
    if (VK_SUCCESS != resultcmd)
    {
        printf("could'nt begin command buffer recording operation\n");
        return false;
    }
    vkCmdCopyBuffer(staging_command_buffer, staging_buffer, staging_dst_buffer, 1, &copyregion);       // copying done
    vkCmdCopyBuffer(staging_command_buffer, index_staging_buffer, index_buffer, 1, &index_copyregion); //
    vkEndCommandBuffer(staging_command_buffer);
    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    VkSubmitInfo staging_submit_info = {};
    staging_submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    staging_submit_info.pNext = NULL;
    staging_submit_info.waitSemaphoreCount = 0;
    staging_submit_info.pWaitSemaphores = NULL;
    staging_submit_info.pWaitDstStageMask = 0;
    staging_submit_info.commandBufferCount = 1;
    staging_submit_info.pCommandBuffers = &staging_command_buffer;
    staging_submit_info.signalSemaphoreCount = 0;
    staging_submit_info.pSignalSemaphores = NULL;
    vkQueueSubmit(graphics_queue, 1, &staging_submit_info, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphics_queue);
    command_buffer = command_buffers[0]; // recording command buffer
    //   std::vector<VkBufferCopy> regions = { {0, destination_offset, data_size} };
    // if (regions.size() > 0)
    //{
    //   vkCmdCopyBuffer(command_buffer, staging_buffer, staging_dst_buffer, static_cast<uint32_t>(regions.size()), &regions[0]);
    //}
    // copying data from buffer to an image
    // VkImage destination_image = 0; // like this is the image in whic buffer data can be stored

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // VkImage source_image = 0;                               // this is the source image  this is image from whic data will be copied
    // VkImageLayout image_layout = VK_IMAGE_LAYOUT_UNDEFINED; // it have the layout which just means how its stored in memory
    // VkOffset3D image_offset = { 0, 0, 0 };
    // std::vector<VkBufferImageCopy> regionsi = { {offset, 0, 0, {aspect, 0, 0, 0}, image_offset, size} };
    // if (regionsi.size() > 0)
    //{
    //   vkCmdCopyBufferToImage(command_buffer, staging_buffer, destination_image, image_layout, static_cast<uint32_t>(regionsi.size()), &regionsi[0]);
    //}
    // e//lse
    //{
    //  return false;
    //}
    // std::vector<VkBufferImageCopy> regionsb = { {memory_offset, 0, 0, {aspect, 0, 0, 0}, image_offset, size} };
    // if (regionsb.size() > 0)
    //{
    //  vkCmdCopyImageToBuffer(command_buffer, source_image, image_layout, staging_dst_buffer, static_cast<uint32_t>(regionsb.size()), &regionsb[0]);
    //}
    // else
    //{
    //  return false;
    //}
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // here i am creating memory barrier for dst
    // NO NEED FOR ALL THESE CAUSEE I ALREADY CREATED IN BUFFER.CPP ONLY CALL HERE
    /* struct BufferTransistion
     { // this struct conatains parametrs we required for creation of memory buffer
         VkBuffer buffer;
         VkAccessFlags CurrentAccess; // this will store information of how the buffer was used and how buffer will be used from now on
         VkAccessFlags NewAcccess;
         uint32_t CurrentQueueFamily; // nothing gors as planned a compute queue may be present in other family
         uint32_t NewQueueFamily;     // so these two parameters is like tranfering ownership from one family to another
     };
     std::vector<BufferTransistion> buffer_transitions;
     buffer_transitions = { {buffer, VK_ACCESS_INDIRECT_COMMAND_READ_BIT, VK_ACCESS_TRANSFER_WRITE_BIT, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED} };
     std::vector<VkBufferMemoryBarrier> buffer_memory_barriers;
     for (auto& buffer_transition : buffer_transitions)
     {
         buffer_memory_barriers.push_back({
             VK_STRUCTURE_TYPE_MEMORY_BARRIER,
             NULL         ,
             buffer_transition.CurrentAccess,
             buffer_transition.NewAcccess,
             buffer_transition.CurrentQueueFamily,
             buffer_transition.NewAcccess,
             buffer_transition.buffer,

             });
     }
     VkPipelineStageFlags generating_stages_before_cmd = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT; // here store values representing pipeline stage that was using before barrier like here generate a barrier
     VkPipelineStageFlags consuming_stages_before_cmd = VK_PIPELINE_STAGE_TRANSFER_BIT;     // here store values representing pipeline stage that will be used after barrier do this after the barrier
     if (buffer_memory_barriers.size() > 0)
     {
         vkCmdPipelineBarrier(command_buffer, generating_stages_before_cmd, consuming_stages_before_cmd, 0, 0, nullptr, static_cast<uint32_t>(buffer_memory_barriers.size()), &buffer_memory_barriers[0], 0, nullptr);
         // memory barriers are used only when we are supposedto wait for operations like if command is given then for the other command i gyesss
     } // vkcmdpipelinebarrier is submitted to in cmdbuffer to queue it just wait other commands within that queue to some point and then executs others like its with queue not like semaphores
     struct BufferTransistion
     { // this struct conatains parametrs we required for creation of memory buffer i guess its for converting that dst to show something case we created it changed it to rad now it will do other thiong
         // HERE IT WILL BE AFTER ENDINBG COMMAND BUFFER LIKE AFTER STAGING I WILL HAVE TO CHANGE THAT DEVICE LOCAL BIT TO SHARE ITS VALUE TO READ VERTX READ SO LIKE THIS DONT TAKKE TENSION ABOUT IT YET
         VkBuffer buffer;
         VkAccessFlags CurrentAccess; // this will store information of how the buffer was used and how buffer will be used from now on
         VkAccessFlags NewAcccess;
         uint32_t CurrentQueueFamily; // nothing gors as planned a compute queue may be present in other family
         uint32_t NewQueueFamily;     // so these two parameters is like tranfering ownership from one family to another
     };
     std::vector<BufferTransistion> buffer_transitions2;
     buffer_transitions = { {buffer, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED} };
     std::vector<VkBufferMemoryBarrier> buffer_memory_barriers2;
     for (auto& buffer_transition : buffer_transitions)
     {
         buffer_memory_barriers.push_back({
             VK_STRUCTURE_TYPE_MEMORY_BARRIER,
             nullptr,
             buffer_transition.CurrentAccess,
             buffer_transition.NewAcccess,
             buffer_transition.CurrentQueueFamily,
             buffer_transition.NewAcccess,
             buffer_transition.buffer,

             });
     }
     VkPipelineStageFlags generating_stages_after_cmd = VK_PIPELINE_STAGE_TRANSFER_BIT;    // here store values representing pipeline stage that was using before barrier like here generate a barrier
     VkPipelineStageFlags consuming_stages_after_cmd = VK_PIPELINE_STAGE_VERTEX_INPUT_BIT; // here store values representing pipeline stage that will be used after barrier do this after the barrier
     if (buffer_memory_barriers2.size() > 0)
     {
         vkCmdPipelineBarrier(command_buffer, generating_stages_after_cmd, consuming_stages_after_cmd, 0, 0, nullptr, static_cast<uint32_t>(buffer_memory_barriers2.size()), &buffer_memory_barriers2[0], 0, nullptr);
         return true;
     }
     else
     {
         return false;
     }*/
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // let us begin render pass
    // PASS THE COPY INSTRUCTION HERE CAUSE THEN THE COPYING IS DONE HERE AFTER THIS POINT
    // SO HERE I AM CREATING BUFFER MEMORY BARRIER ITS CREATION CODE IS IN BUFFER.CPP I AM CALLING CMDBUFFER FUNCTION HERE
    return true;
}
bool renderloop()
{
    VkRect2D render_area = (VkRect2D){0, 0, 0, 0};
    render_area.offset = (VkOffset2D){0, 0};
    render_area.extent.height = size_of_images.height;
    render_area.extent.width = size_of_images.width;
    VkClearValue clear_value = {};
    clear_value.color = (VkClearColorValue){0.0f, 0.0f, 0.0f, 0.1f};
    VkClearValue clear_values[] = {clear_value};
    VkSubpassContents subpass_contents = VK_SUBPASS_CONTENTS_INLINE;
    VkRenderPassBeginInfo render_pass_begin_info = {
        VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        NULL,
        render_pass,                          // i will get it from render pass so wait
        swapchain_frame_buffers[image_index], // i will get it from render pass so wait
        render_area,
        1,
        clear_values};
    VkSubmitInfo submit_info;
    waitsemaphores = malloc(sizeof(VkSemaphore) * 1);
    waitsemaphores[0] = imageavailablesemaphore; // i will wait for image available semaphore which is signaled from vkacquirenextimagekhr just assigning it the first index of my malloc cause why not
    VkPipelineStageFlags waitstages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    // HERE SIGNALSEMAPHORE I WILL EXTERN IT CAUSE MY PRESENT WILL PROBABLY USE IT
    // std::vector<VkSemaphore>signalsemaphores;
    VkCommandBufferResetFlags release_resources = VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT;
    resultcmd = vkResetCommandBuffer(command_buffers[image_index], release_resources ? VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT : 0);
    if (VK_SUCCESS != resultcmd)
    {
        printf("could'nt RESET command buffer recording operation\n");
        return false;
    }
    VkCommandBufferBeginInfo command_buffer_begin_info = {
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        NULL,
        usage,
        secondary_command_buffer_info};
    resultcmd = vkBeginCommandBuffer(command_buffers[image_index], &command_buffer_begin_info);
    if (VK_SUCCESS != resultcmd)
    {
        printf("could'nt begin command buffer recording operation\n");
        return false;
    }

    vkCmdBeginRenderPass(command_buffers[image_index], &render_pass_begin_info, subpass_contents);
    // vkCmdNextSubpass(command_buffer, subpass_contents);
    vkCmdBindPipeline(command_buffers[image_index],
                      VK_PIPELINE_BIND_POINT_GRAPHICS,
                      pipe); // pipe is created in pipeline.c and its global so i can use it here
    VkDeviceSize offsets0[] = {0};

    vkCmdBindVertexBuffers(
        command_buffers[image_index],
        0,
        1,
        &staging_dst_buffer,
        offsets0);
    vkCmdBindIndexBuffer(command_buffers[image_index], index_buffer, 0, VK_INDEX_TYPE_UINT32);
    vkCmdDrawIndexed(command_buffers[image_index], 6, 1, 0, 0, 0);    


    vkCmdEndRenderPass(command_buffers[image_index]); // idk wtf i did but i will come back when debugging after writing shaders or adfter pipeluine then bye---------------------------------------------------------
    vkEndCommandBuffer(command_buffers[image_index]);

    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.pNext = NULL;
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = &waitsemaphores[0]; // how is this invalid semaphore
    submit_info.pWaitDstStageMask = waitstages;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffers[image_index];
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = &renderfinishedsemaphore;      // signal render finished semaphore from here and now vkqueuepresent will wait on ok frim this point it will go to present .c  to help in swapchain present
    vkQueueSubmit(graphics_queue, 1, &submit_info, inflightfence); ////////////////////////////here claude plszzz hlp
    vkQueueWaitIdle(graphics_queue);
    return true;
}
/*bool creatingsemaphore()
{
    using namespace VulkanCookbook;
    VkSemaphoreCreateInfo semaphore_create_info = {
        VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        nullptr,
        0 };
    resultcmd = vkCreateSemaphore(logical_device, &semaphore_create_info, nullptr, &semaphorecmd);
    if (VK_SUCCESS != resultcmd)
    {
        std::cout << "error creating semaphore<\n";
        return false;
    }
    return true;
}
bool creatingfence()                // same like semaphore but here the commands after getting executed from queue to gpu if we want the cpu to know that this command is executed we can use fence
{                                   // it also have sign aled and unsignaled condition example we have cmd buffer it executes in transfer queue and along with it fence is attached if we take a screenshot
    using namespace VulkanCookbook; // now untill the fence is signaled which will happen after that queue is executed untill then cpu waits idle so we after have screeenshot with correct sequqence
    VkFenceCreateInfo fence_create_info = {
        VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        nullptr,
        0 };
    resultcmd = vkCreateFence(logical_device, &fence_create_info, nullptr, &fencecmd);
    if (VK_SUCCESS != resultcmd)
    {
        std::cout << "could'nt create fence\n";
        return false;
    }

    return true;
}
//HOOOOOOOO I WILL DO THIS STUFF AFTER WARDS FOR NOW JUST LETS DO LIKE NORMAL WITHOUT ANY SEMAPHORE SUBMIT
/*
bool submitting()
{
    using namespace VulkanCookbook;
    struct WaitSemaphoreInfo
    {                          // its custom struct in which we give semaphore handles of semaphores through which hardware should wait // idkk kkkkk
        VkSemaphore Semaphore; // this is already given so just think or ignore it
        VkPipelineStageFlags WaitingStage;
    };
    // here we werre supposedd to take the queue in which command buffers should b added but we already declared graphics queue and its the queue so dont rewrite it
    std::vector<WaitSemaphoreInfo> wait_semaphore_infos; // here there are two and its struct which means container so its obviopus you should make conatiner
    wait_semaphore_infos = {
        {semaphorecmd, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT} };
    std::vector<VkSemaphore> wait_semaphore_handles;         // just this is the semaphores which make wait other queues to wait untill one is finished
    std::vector<VkPipelineStageFlags> wait_semaphore_stages; // this means in pipeline do the functions but wait at some point so that every other things which are before are completed correctly
    // Initialize the vector with one or more WaitSemaphoreInfo objects

    for (auto& wait_semaphore_info : wait_semaphore_infos)
    {
        wait_semaphore_handles.emplace_back(wait_semaphore_info.Semaphore);
        wait_semaphore_stages.emplace_back(wait_semaphore_info.WaitingStage);
    }
    std::vector<VkSemaphore> signal_semaphores; // for storing all semaphores which will have all signal semaphores

    VkSubmitInfo submit_info{
        VK_STRUCTURE_TYPE_SUBMIT_INFO,
        nullptr,
        static_cast<uint32_t>(wait_semaphore_handles.size()),
        wait_semaphore_handles.size() > 0 ? &wait_semaphore_handles[0] : nullptr,
        wait_semaphore_stages.size() > 0 ? &wait_semaphore_stages[0] : nullptr,
        static_cast<uint32_t>(command_buffers.size()),
        command_buffers.size() > 0 ? &command_buffers[0] : nullptr,
        static_cast<uint32_t>(signal_semaphores.size()),
        signal_semaphores.size() > 0 ? &signal_semaphores[0] : nullptr };
    resultcmd = vkQueueSubmit(graphics_queue, 1, &submit_info, fencecmd);
    if (VK_SUCCESS != resultcmd)
    {
        std::cout << "cant submit cmd buffer\n";
        return false;
    }
    VkCommandBufferResetFlags release_resources = VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT;
    resultcmd = vkResetCommandBuffer(command_buffer, release_resources ? VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT : 0);
    if (VK_SUCCESS != resultcmd)
    {
        std::cout << "could'nt RESET command buffer recording operation\n";
        return false;
    }
    VkCommandBufferResetFlags release_resourcesP = VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT;
    resultcmd = vkResetCommandPool(logical_device, command_pool, release_resourcesP ? VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT : 0);
    if (VK_SUCCESS != resultcmd)
    {
        std::cout << "could'nt RESET command POOL recording operation\n";
        return false;
    }
    // we are supposed to use wait and resset between coommands cause what will you wait or check when there is no command
    std::vector<VkFence> fences{ fencecmd }; // now just make cpu wait for fences its bad practice cause you are making cpu wait to get signal when the first buffer is running
    // but let us just use it cause cause idk how others work
    VkBool32 wait_for_all = VK_FALSE; // here if it was false then even one is complted then it would go to cpu even if others are still in progress
    uint32_t timeout = 1000000;       // threshold limit if it crosses this then error timeout will come if 0 then we can see signlaiibng proceess of fenced

    // synchronizing two command buffers think like before submitting to queues cnda and cmdb are not executed or sent to queue in correct manner
    // so we can use semaphore to tell cmdb to wait for signal of cmda then start
    VkQueue first_queue = graphics_queue;
    std::vector<WaitSemaphoreInfo> synchronizing_semaphore;
    // can do this way but i guess it may cause mismatch so do this way synchronizing_semaphore.emplace_back(semaphore);//here we are giving our semaphore which was created earlier to this waitsemaphoreinfo cause
    // above this says store a semaphore which should be signaled to cmdb after completion so this is that semaphore which is going to be signaled
    std::vector<VkSemaphore> first_signal_semaphores; // same we will emplace back semaphore dont worry
    for (auto& semaphore_info : synchronizing_semaphore)
    {
        first_signal_semaphores.emplace_back(semaphore_info.Semaphore); // here idk why but we gone to struct then this then taht then finally submitted to vksemaphore only
    }

    VkSubmitInfo first_submit_info{                               // here we are submitting two cmdbuffer to queue which is graphics queue i guess we have two queues one graphics and one compute
                                   VK_STRUCTURE_TYPE_SUBMIT_INFO, // so less go synchronization with semaphore is mandatory
                                   nullptr,                       // even if they were in same queue we should have gone to synchronization with semaphore cause vulkan gurantees that command buufer are
                                   0,                             // submitted in order but who said they will executein correct order
                                   nullptr,                       // so if we synchronized there will be better flow so that second starts after one command so no tearing
                                   nullptr,
                                   static_cast<uint32_t>(command_buffers.size()),
                                   command_buffers.size() > 0 ? &command_buffers[0] : nullptr,
                                   static_cast<uint32_t>(first_signal_semaphores.size()),
                                   first_signal_semaphores.size() > 0 ? &first_signal_semaphores[0] : nullptr };
    resultcmd = vkQueueSubmit(first_queue, 1, &first_submit_info, fencecmd); // here we are submititing our first command buffer to queue see it saying it also have fence if we wanted just gpu to do work
    if (VK_SUCCESS != resultcmd)                                          // then null would have been choice cause we dont really need fence here cause nothing is going to cpu side like cause we are just
    {                                                                  // giving command to queue after that that quueue will execute we will need fence i guess but i dont know why to use fence here
        std::cout << "cant submit 1 cmd buffer\n";                     // so just think it like this this fence is unsignaled now but when submition happens
        return false;                                                  // fence will become signaled so we will why do we need to vkwaitforfence and reset remove that fucking thing
    }
    if (fences.size() > 0) // here we are waiting for fence to get signaled and lettting cpu also wait untill this queue is done
    {
        resultcmd = vkWaitForFences(logical_device, static_cast<uint32_t>(fences.size()), &fences[0], wait_for_all, timeout); // bad cause cpu will wait untill this cmd gets executes waste of time
        if (VK_SUCCESS != resultcmd)
        { // idk why just assume that cpu has no other tassks;
            std::cout << " waiting for fences failed you have " << fences.size() << "   " << fencecmd << " fence " << "\n";
            return false;
        }
        else
        {
            std::cout << " you waited for fence " << fences.size();
            return true;
        }
    }
    else
    {
        std::cout << "you have no fence to begin with you have " << fences.size() << "      " << fencecmd << " fence" << "\n";
        return false;
    }
    // there is vkqueuewaitidle vkdevicewaitidle but they are just waiste so i will not include them



    VkQueue second_queue = compute_queue;
    std::vector<VkSemaphore> wait_signal_semaphores;
    std::vector<VkPipelineStageFlags> wait_pipeline_stages;
    for (auto& semaphore_info_2 : synchronizing_semaphore)
    {
        wait_signal_semaphores.emplace_back(semaphore_info_2.Semaphore);  // with this we are giving same semaphore to waiting cause thats the semaphore which we gave 1submit
        wait_pipeline_stages.emplace_back(semaphore_info_2.WaitingStage); // here its pipeline stage so this just helps means at some point its will stop in pipeline and will wait untill first semaphore is signaled
    }
    VkSubmitInfo second_submit_info{// here submitting other command which is going to command queue all computations are done here
                                    VK_STRUCTURE_TYPE_SUBMIT_INFO,
                                    nullptr,
                                    static_cast<uint32_t>(wait_signal_semaphores.size()), // here its waiting for first semaphore to come and give signal we are using same semaphore struct see
                                    wait_signal_semaphores.size() > 0 ? &wait_signal_semaphores[0] : nullptr,
                                    wait_pipeline_stages.size() > 0 ? &wait_pipeline_stages[0] : nullptr, // at what stage you want the execution to stop untill you get the thing from first
                                    static_cast<uint32_t>(command_buffers.size()),
                                    command_buffers.size() > 0 ? &command_buffers[0] : nullptr,
                                    0,
                                    nullptr };
    resultcmd = vkQueueSubmit(second_queue, 1, &second_submit_info, fencecmd); // so now the fence we used is in its previous state it doent reset like semaphore
    if (VK_SUCCESS != resultcmd)
    {
        std::cout << "cant submit 2 cmd buffer\n";
        return false;
    }
    else
    {
        return true;
    }
    if (fences.size() > 0) // we are reseting fence so we can just use this fence when we want other fences like why to waste memory in creating more fences
    {
        resultcmd = vkResetFences(logical_device, static_cast<uint32_t>(fences.size()), &fences[0]); // unlike semaphore fences has to be resetted after being signaled they dont go back to unsignald phase like semaphores dids
        if (VK_SUCCESS != resultcmd)
        {
            std::cout << " reseting for fences failed\n";
            return false;
        }
        return true;
    }
    else
    {
        std::cout << "you have no fence to begin with you have " << fences.size() << " fence" << "\n";
        return false;
    }
    return true;
}
bool destroyingfence()
{ // dont call this in main
    using namespace VulkanCookbook;
    if (VK_NULL_HANDLE != fencecmd)
    {

        vkDestroyFence(logical_device, fencecmd, nullptr);
        fencecmd = VK_NULL_HANDLE;
    }
    return true;
}
bool destroyingSemaphore()
{
    using namespace VulkanCookbook;
    if (VK_NULL_HANDLE != fencecmd)
    {

        vkDestroySemaphore(logical_device, semaphorecmd, nullptr);
        semaphorecmd = VK_NULL_HANDLE;
    }
    return true;
}
bool destroyingcmd()
{
    using namespace VulkanCookbook;
    if (command_buffers.size() > 0)
    {

        vkFreeCommandBuffers(logical_device, command_pool, command_buffers.size(), &command_buffers[0]);
        command_buffers.clear();
    }
    return true;
}
bool destroyingcmdpool()
{
    using namespace VulkanCookbook;
    if (VK_NULL_HANDLE != command_pool)
    {

        vkDestroyCommandPool(logical_device, command_pool, nullptr);
        command_pool = VK_NULL_HANDLE;
    }
    return true;
}*/
void cmdbuffer_cleanup()
{

    vkFreeCommandBuffers(logical_device, command_pool, 1, command_buffers);
    vkFreeCommandBuffers(logical_device, command_pool, 1, staging_command_buffers);

    vkDestroyCommandPool(logical_device, command_pool, NULL);
    free(waitsemaphores);
}