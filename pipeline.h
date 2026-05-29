#pragma once
#include <stdio.h>
#include <stdbool.h>
#include <vulkan/vulkan.h>
bool creatingshadermodule();
bool specifyingpipelineshaderstages();
bool vertexbinding();
bool pipelineinputassemblystate();
bool pipelineviewport();
bool specifyingpipelinerasterization();
bool pipelinemultisamplestate();
bool pipelineblendstate();
bool creatingpipeline();
void pipeline_cleanup();
extern VkPipeline pipe;