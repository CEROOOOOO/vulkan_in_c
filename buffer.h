#pragma once
#include <stdio.h>
#include <vulkan/vulkan.h>
#include <stdbool.h>
extern VkBuffer buffer;
extern VkBuffer uniform_texel_buffer;
extern VkBuffer storage_texel_buffer;
extern VkBuffer staging_buffer;
extern VkBuffer staging_dst_buffer;
extern VkMemoryRequirements memory_requirements;                           // memory requirements we get requirements of that memory properties
extern VkMemoryRequirements memory_requirements_for_staging;               // requirements i will need for staging buffer
extern VkMemoryRequirements memory_requirements_for_dst_staging;           // requirements i will need for staging buffer
extern VkMemoryRequirements memory_requirements_for_uniform_texel;         // requirements i will need for staging buffer
extern VkMemoryRequirements memory_requirements_for_storage_texel;         // requirements i will need for staging buffer
extern VkDeviceMemory memory_object ;                      // idk but object should be null pointer
extern VkDeviceMemory memory_object_for_staging ;          // idk but object should be null pointer when we call vkallocate memory it gets populatec with everything we want like from now on we will use this
extern VkDeviceMemory memory_object_for_dst_staging ;      // idk but object should be null pointer
extern VkDeviceMemory memory_object_for_storage_image ;    // idk but object should be null pointer
extern VkDeviceMemory memory_object_for_attachment_image ; // idk but object should be null pointer
extern VkDeviceMemory memory_object_for_uniform_texel;    // idk but object should be null pointer
extern VkDeviceMemory memory_object_for_storage_texel ;    // idk but object should be null pointer
extern VkMemoryPropertyFlagBits memory_properties ;
extern VkMemoryPropertyFlagBits memory_property_for_staging ;
extern VkMemoryPropertyFlagBits memory_property_for_dst_staging;
extern VkMemoryPropertyFlagBits memory_property_for_uniform_texel;
extern VkMemoryPropertyFlagBits memory_property_for_storage_texel ;
extern VkPhysicalDeviceMemoryProperties physical_device_memory_properties;
extern VkFormat format ; // how the buffer should be viewed
extern VkDeviceSize memory_offset;            // starting point of buffer memory from which the buffer view should be created its our destination its in gpu side
extern VkDeviceSize memory_range; // its the size if buffer view which we created
extern VkSemaphore semaphorecmd;
extern VkFence fencecmd;
extern VkCommandBuffer cmdbufferforstaging;
extern VkCommandPool cmdpoolforstaging;
extern     VkBufferCopy copyregion;
bool creatingcmdforstaging();
extern VkCommandBuffer *cmdbuffers;
extern VkBufferMemoryBarrier* buffer_memory_barriers;
extern VkPipelineStageFlags generating_stages;
extern VkPipelineStageFlags consuming_stgaes;
bool creatingbuffer();
bool allocatingandbindingmemory();
bool stagingresources();
bool creatingbufferview();
void buffer_cleanup();
extern VkDeviceSize offset;                       // memory region that should be mapped and packed from cpu its our source which will be from cpu
extern VkDeviceSize data_size;                    // size of data to be packed from cpu to the memory object of gp
extern VkBufferCopy copyregion;
bool copyingvertexbuffer();
extern VkBuffer index_buffer;
extern VkBuffer index_staging_buffer;
extern VkBufferCopy index_copyregion;
extern VkVertexInputBindingDescription* binding_descriptions;
extern VkVertexInputAttributeDescription* attribute_descriptions;
bool bufferbinding();
bool freeingcmd(); 