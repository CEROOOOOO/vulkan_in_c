#include <stdio.h>
#include <stdbool.h>
#include "instance.h"
#include "device.h"
#include "present.h"
#include "buffer.h"
#include "renderpass.h"
#include "cmdbuffer.h"
#include "pipeline.h"
#include <GLFW/glfw3.h>

VkResult result = VK_SUCCESS;
int main()
{
    glfwInit();
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if (checkvalidationlayersupport())printf("fuck you 1\n");
    if (createinstance())printf("fuck you 3\n");
    //               device                      //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if (enumeratingavailablephysicaldevice())printf("fuck you 4\n");
    if (loadinglogicaldevice())printf("fuck you 5\n");
    if (gettingqueue())printf("fuck you 6\n");
    //                presentation                 //
    if (presentingsurface())printf("fuck you 6.5");
    if (selectingqueuefamilythatsupportspresentationforgivensurface())printf("fuck you 7\n");
    if (selectingdesiredpresentationmode())printf("fuck you 8\n");
    if (capabilitiesofpresentationsurface())printf("fuck you 9\n");
    if (selectingnumberofswapchainimages())printf("fuck you 10\n");
    if (choosingsizeofswapchainimages())printf("fuck you 11\n");
    if (desiredusageofswapchainimages())printf("fuck you 12\n");
    if (transformationofswapchainimages())printf("fuck you 13\n");
    if (createswapchain())printf("fuck you 14\n");
    if (gettinghandlesofswapchain())printf("fuck you 15\n");
    if (creatingswapchainimageviews())printf("fuck you 16\n");
    if (creatingsemaphoreimg())printf("fuck you 17\n");
    if (creatingfenceimg())printf("fuck you 18\n");
    if (creatingcmdforstaging())printf("fuck you 19\n");
    if (creatingbuffer())printf("fuck you 20\n");
    if (allocatingandbindingmemory())printf("fuck you 21\n");
    if (stagingresources())printf("fuck you 22\n");
    if (copyingvertexbuffer())printf("fuck you 23\n");
    if (renderpassandframebuffer())printf("fuck you 24\n");
    if (creatingcommand())printf("fuck you 25\n");
    if (creatingshadermodule())printf("fuck you 26\n");
    if (specifyingpipelineshaderstages())printf("fuck you 27\n");
    if (vertexbinding())printf("fuck you 28\n");
    if (pipelineinputassemblystate())printf("fuck you 29\n");
    if (pipelineviewport())printf("fuck you 30\n");
    if (specifyingpipelinerasterization())printf("fuck you 31\n");
    if (pipelinemultisamplestate())printf("fuck you 32\n");
    if (pipelineblendstate())printf("fuck you 33\n");
    if (creatingpipeline())printf("fuck you 34\n");
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        acquiringswapchainimages();
        renderloop();
        presentinganimage();
    }
// freeing task of all mallocs i did
    pipeline_cleanup();
    renderpass_cleanup();
    cmdbuffer_cleanup();
    buffer_cleanup();
    present_cleanup();
    device_cleanup(); 
    instance_cleanup();
    //  if(creatingbufferview())std::cout<<"fuck you26\n";
      //JUST FORGET ABOUT IMAGE BUFFER LIKE I AM NIT RENDERING ANY TEXTURE YET SO WILL JUST USE BUFFER THATS IT
      //    //if(creatinganimage())std::cout<<"fuck you27\n";
      //if(allocatingandbinding())std::cout<<"fuck you28\n";
      //if(settingimagememorybarrier())std::cout<<"fuck you29\n";
      //if(creatingimageview())std::cout<<"fuck you30\n";
      //if(stagingresources())std::cout<<"fuck you31\n";
      //if(creatingsampler())std::cout<< "fuck you32\n";
      //if (creatingsampledimage())std::cout << "fuck you33\n";
      //if(descriptorsetlayout())std::cout<<"fuck you34\n";
      //if(descriptorpool())std::cout<<"fuck you35\n";
      //if(updatingdescriptorsets())std::cout<<"fuck you36\n";
      ///these two are supposed to be in loop 
}