#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <vulkan/vulkan.h>
#include "device.h"
#include "renderpass.h"
#include "present.h"
#include "buffer.h"
VkShaderModule vertex_shader_module;
VkShaderModule fragment_shader_module;
VkResult resultpipe;
	typedef struct {
		VkShaderStageFlagBits shaderstage;
		VkShaderModule shadermodule;
		char const* entrypointname;
		VkSpecializationInfo const* specializationinfo;
	}ShaderStageParameters;
VkPipelineShaderStageCreateInfo* shader_stage_create_infos = NULL;
 ShaderStageParameters* shader_stage_params = NULL;
VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info = {};
VkPipelineInputAssemblyStateCreateInfo input_assembly_state_create_info = {};
VkPipelineViewportStateCreateInfo viewport_state_create_info = {};
VkPipelineRasterizationStateCreateInfo rasterization_state_create_info = {};
VkPipelineMultisampleStateCreateInfo multisample_state_create_info = {};
VkPipelineColorBlendStateCreateInfo color_blend_state_create_info = {};
VkPipeline pipe;
VkPipelineLayout pipe_layout;
VkViewport viewports = {};
VkRect2D scissor = {};
VkPipelineColorBlendAttachmentState
color_blend_attach;
uint32_t* readSPIRV(const char* filename, size_t* out_size) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        printf("Failed to open SPIR-V file: %s\n", filename);
        return NULL;
    }
    fseek(file, 0, SEEK_END);
	size_t fileSize = ftell(file);//size of the whole file 
    rewind(file);//idk wtf 
	uint32_t* bufferfile = malloc(fileSize);
    if (!bufferfile) {
        printf("Failed to allocate memory for SPIR-V file: %s\n", filename);
        fclose(file);
        return NULL;
    }
    fread(bufferfile, 1, fileSize, file);//read that file and store it in buffer 
    fclose(file);
    //free(bufferfile);//i think i cant do this 
    *out_size = fileSize;
    return bufferfile;
}
bool creatingshadermodule()//like this is the storage of shader means spirv is stored here and from here we can send it to pipeline 
{
	size_t vertex_code_size;
	size_t fragment_code_size;
	//std::vector <unsigned char> vertex_source_code = readSPIRV("vertx2.spv");//i dont think i need a array cause i have only one vert
	uint32_t* vertex_source_code = readSPIRV("vertex1.spv", &vertex_code_size);// i have not created anything though wait 
if (!vertex_source_code) {
    printf("failed to load vertex shader\n");
    return false;
}

	VkShaderModuleCreateInfo vertex_shader_module_create_info = {};
	vertex_shader_module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	vertex_shader_module_create_info.pNext = NULL;
	vertex_shader_module_create_info.flags = 0;
	vertex_shader_module_create_info.codeSize = vertex_code_size;
	vertex_shader_module_create_info.pCode = vertex_source_code;
	resultpipe = vkCreateShaderModule(logical_device, &vertex_shader_module_create_info, NULL, &vertex_shader_module);
	if (VK_SUCCESS != resultpipe)
	{
	printf("couldnt create shader module\n");
	return false;
	}
	uint32_t* fragment_source_code = readSPIRV("fragment1.spv", &fragment_code_size);
if (!fragment_source_code) {
    printf("failed to load frag shader\n");
    return false;
}
	VkShaderModuleCreateInfo fragment_shader_module_create_info = {};
	fragment_shader_module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	fragment_shader_module_create_info.pNext = NULL;
	fragment_shader_module_create_info.flags = 0;
	fragment_shader_module_create_info.codeSize = fragment_code_size;
	fragment_shader_module_create_info.pCode = fragment_source_code;
	resultpipe = vkCreateShaderModule(logical_device, &fragment_shader_module_create_info, NULL, &fragment_shader_module);
	if (VK_SUCCESS != resultpipe)
	{
		printf("couldnt create shader module\n");
		return false;
	}
	return true;

}
bool specifyingpipelineshaderstages()//like in graphics shaders there are many shaders like vert frag blah blah so this to specify 
{
    shader_stage_params = malloc(sizeof(ShaderStageParameters)*2);
    shader_stage_params[0] = (ShaderStageParameters){VK_SHADER_STAGE_VERTEX_BIT, vertex_shader_module, "main", NULL};
    shader_stage_params[1] = (ShaderStageParameters){VK_SHADER_STAGE_FRAGMENT_BIT, fragment_shader_module, "main", NULL};
    shader_stage_create_infos = malloc(sizeof(VkPipelineShaderStageCreateInfo)*3);
    for(int i = 0 ; i < 2; i++){
        shader_stage_create_infos[i] = (VkPipelineShaderStageCreateInfo){
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            NULL,
            0,
            shader_stage_params[i].shaderstage,
            shader_stage_params[i].shadermodule,
            shader_stage_params[i].entrypointname,
            shader_stage_params[i].specializationinfo
        };
    }
	return true;
}
bool vertexbinding()//like to specify from where to take vertex data
{
	vertex_input_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertex_input_state_create_info.pNext = NULL;
	vertex_input_state_create_info.flags = 0;
	vertex_input_state_create_info.vertexBindingDescriptionCount = 1;
	vertex_input_state_create_info.pVertexBindingDescriptions = binding_descriptions;
	vertex_input_state_create_info.vertexAttributeDescriptionCount = 2;
	vertex_input_state_create_info.pVertexAttributeDescriptions = attribute_descriptions;
	return true;
}
bool pipelineinputassemblystate() {
	input_assembly_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	input_assembly_state_create_info.pNext = NULL;
	input_assembly_state_create_info.flags = 0;
	input_assembly_state_create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	input_assembly_state_create_info.primitiveRestartEnable = VK_FALSE;
	return true;
}
bool pipelineviewport() {
	viewports.x = 0.0f;
	viewports.y = 0.0f;
	viewports.width = (float)size_of_images.width;
	viewports.height = (float)size_of_images.height;
	viewports.minDepth = 0.0f;
	viewports.maxDepth = 1.0f;

	scissor.offset = (VkOffset2D){ 0, 0 };
	scissor.extent.height = size_of_images.height;
	scissor.extent.width = size_of_images.width;

		viewport_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO; 
		viewport_state_create_info.pNext = NULL;
		viewport_state_create_info.flags = 0;
		viewport_state_create_info.viewportCount = 1;
		viewport_state_create_info.pViewports = &viewports;
		viewport_state_create_info.scissorCount = 1;
		viewport_state_create_info.pScissors = &scissor;
		return true;

}
bool specifyingpipelinerasterization()//generates fragments
{
	rasterization_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterization_state_create_info.pNext = NULL;
	rasterization_state_create_info.flags = 0;
	rasterization_state_create_info.depthClampEnable = VK_FALSE;
	rasterization_state_create_info.rasterizerDiscardEnable = VK_FALSE;
	rasterization_state_create_info.polygonMode = VK_POLYGON_MODE_FILL;
	rasterization_state_create_info.cullMode = VK_CULL_MODE_NONE;
	rasterization_state_create_info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterization_state_create_info.depthBiasEnable = VK_FALSE;
	rasterization_state_create_info.depthBiasConstantFactor = 0.0f;
	rasterization_state_create_info.depthBiasClamp = 0.0f;
	rasterization_state_create_info.depthBiasSlopeFactor = 0.0f;
	rasterization_state_create_info.lineWidth = 1.0f;
	return true;

}
bool pipelinemultisamplestate() {
	multisample_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisample_state_create_info.pNext = NULL;
	multisample_state_create_info.flags = 0;
	multisample_state_create_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisample_state_create_info.sampleShadingEnable = VK_FALSE;
	multisample_state_create_info.minSampleShading = 1.0f;
	multisample_state_create_info.pSampleMask = NULL;
	multisample_state_create_info.alphaToCoverageEnable = VK_FALSE;
	multisample_state_create_info.alphaToOneEnable = VK_FALSE;
	return true;
}
bool pipelineblendstate()
{

	color_blend_attach
		.blendEnable = VK_FALSE;
	color_blend_attach
		.srcColorBlendFactor =
		VK_BLEND_FACTOR_ONE;
	color_blend_attach
		.dstColorBlendFactor =
		VK_BLEND_FACTOR_ZERO;
	color_blend_attach
		.colorBlendOp =
		VK_BLEND_OP_ADD;
	color_blend_attach
		.srcAlphaBlendFactor =
		VK_BLEND_FACTOR_ONE;
	color_blend_attach
		.dstAlphaBlendFactor =
		VK_BLEND_FACTOR_ZERO;
	color_blend_attach
		.alphaBlendOp =
		VK_BLEND_OP_ADD;
	color_blend_attach
		.colorWriteMask
		= VK_COLOR_COMPONENT_R_BIT
		| VK_COLOR_COMPONENT_G_BIT
		| VK_COLOR_COMPONENT_B_BIT
		| VK_COLOR_COMPONENT_A_BIT;
	printf("color blend attachment state filled.\n");
	color_blend_state_create_info.sType =
		VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	color_blend_state_create_info.pNext = NULL;
	color_blend_state_create_info.flags = 0;
	color_blend_state_create_info
		.logicOpEnable = VK_FALSE;
	color_blend_state_create_info
		.logicOp = VK_LOGIC_OP_COPY;
	color_blend_state_create_info
		.attachmentCount = 1;
	color_blend_state_create_info
		.pAttachments =
		&color_blend_attach;
	for (uint32_t i = 0; i < 4; i++) {
		color_blend_state_create_info.
			blendConstants[i] = 0.0f;
	}
	printf("color blend state info filled.\n");
	return true;

}
bool creatingpipeline() {
	VkPipelineLayoutCreateInfo
		pipe_lay_cre_info;
	pipe_lay_cre_info.sType =
		VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipe_lay_cre_info.pNext = NULL;
	pipe_lay_cre_info.flags = 0;
	pipe_lay_cre_info.setLayoutCount = 0;
	pipe_lay_cre_info.pSetLayouts = NULL;
	pipe_lay_cre_info
		.pushConstantRangeCount = 0;
	pipe_lay_cre_info
		.pPushConstantRanges = NULL;

	resultpipe = vkCreatePipelineLayout(logical_device, &pipe_lay_cre_info, NULL, &pipe_layout);
	if(VK_SUCCESS != resultpipe)
	{
		printf("error in pipeline layout \n");
		return false;
	}
	//
	//create pipeline
	//
	VkGraphicsPipelineCreateInfo
		pipe_cre_info = {};
	pipe_cre_info.sType =
		VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipe_cre_info.pNext = NULL;
	pipe_cre_info.flags = 0;
	pipe_cre_info.stageCount = 2;
	pipe_cre_info.pStages =
		shader_stage_create_infos;
	pipe_cre_info.pVertexInputState = 
		&vertex_input_state_create_info;
	pipe_cre_info.pInputAssemblyState =
		&input_assembly_state_create_info;
	pipe_cre_info.pTessellationState = NULL;
	pipe_cre_info.pViewportState =
		&viewport_state_create_info;
	pipe_cre_info.pRasterizationState =
		&rasterization_state_create_info;
	pipe_cre_info.pMultisampleState =
		&multisample_state_create_info;
	pipe_cre_info.pDepthStencilState = NULL;
	pipe_cre_info.pColorBlendState =
		&color_blend_state_create_info;
	pipe_cre_info.pDynamicState = NULL;

	pipe_cre_info.layout = pipe_layout;
	pipe_cre_info.renderPass = render_pass;
	pipe_cre_info.subpass = 0;
	pipe_cre_info.basePipelineHandle =
		VK_NULL_HANDLE;
	pipe_cre_info.basePipelineIndex = -1;


	resultpipe = vkCreateGraphicsPipelines(logical_device, VK_NULL_HANDLE, 1, &pipe_cre_info, NULL, &pipe);
	if (VK_SUCCESS != resultpipe)
	{
		printf("error creating graphics pipeline\n");
		return false;
	}
	//
	return true;
}
void pipeline_cleanup()
{
    free(shader_stage_create_infos);
    free(shader_stage_params);
    vkDestroyPipeline(logical_device, pipe, NULL);
    vkDestroyPipelineLayout(logical_device, pipe_layout, NULL);
    vkDestroyShaderModule(logical_device, vertex_shader_module, NULL);
    vkDestroyShaderModule(logical_device, fragment_shader_module, NULL);
    vkDestroyDevice(logical_device, NULL);
}