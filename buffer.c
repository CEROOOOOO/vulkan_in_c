#include <stdio.h>
#include <stdlib.h>
#include <vulkan/vulkan.h>
#include "present.h"
#include "device.h"
#include <string.h>
VkBuffer buffer;               // this too remove
VkBuffer uniform_texel_buffer; // dont care
VkBuffer storage_texel_buffer; // dont care
VkBuffer staging_buffer;       // sufficient
VkBuffer staging_dst_buffer;
VkBuffer index_buffer;
VkBuffer index_staging_buffer;
VkPhysicalDeviceMemoryProperties physical_device_memory_properties;
VkMemoryRequirements memory_requirements;                           // memory requirements we get requirements of that memory properties
VkMemoryRequirements memory_requirements_for_staging;               // requirements i will need for staging buffer
VkMemoryRequirements memory_requirements_for_dst_staging;           // requirements i will need for staging buffer
VkMemoryRequirements memory_requirements_for_uniform_texel;         // requirements i will need for staging buffer
VkMemoryRequirements memory_requirements_for_storage_texel;         // requirements i will need for staging buffer
VkMemoryRequirements memory_requirements_for_index_staging;         // bro like this will give my cpu value to gpy its a temporary sacrifice
VkMemoryRequirements memory_requirements_for_index_dst;             // like this with some magic this will hold data in gpu
VkDeviceMemory memory_object = VK_NULL_HANDLE;                      // idk but object should be null pointer
VkDeviceMemory memory_object_for_staging = VK_NULL_HANDLE;          // idk but object should be null pointer when we call vkallocate memory it gets populatec with everything we want like from now on we will use this
VkDeviceMemory memory_object_for_dst_staging = VK_NULL_HANDLE;      // idk but object should be null pointer
VkDeviceMemory memory_object_for_storage_image = VK_NULL_HANDLE;    // idk but object should be null pointer
VkDeviceMemory memory_object_for_attachment_image = VK_NULL_HANDLE; // idk but object should be null pointer
VkDeviceMemory memory_object_for_uniform_texel = VK_NULL_HANDLE;    // idk but object should be null pointer
VkDeviceMemory memory_object_for_storage_texel = VK_NULL_HANDLE;    // idk but object should be null pointer
VkDeviceMemory memory_object_for_index_staging;                     // bro like this will give my cpu value to gpy its a temporary sacrifice
VkDeviceMemory memory_object_for_index_dst;                         // like this with some magic this will hold data in gpu
VkMemoryPropertyFlagBits memory_properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
VkMemoryPropertyFlagBits memory_property_for_staging = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
VkMemoryPropertyFlagBits memory_property_for_dst_staging = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
VkMemoryPropertyFlagBits memory_property_for_uniform_texel = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
VkMemoryPropertyFlagBits memory_property_for_storage_texel = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
VkMemoryPropertyFlagBits memory_property_for_index_staging = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
VkMemoryPropertyFlagBits memory_property_for_index_dst = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
VkDeviceSize memory_offset;
VkDeviceSize memory_range;
VkFormat format = VK_FORMAT_R8G8B8A8_UNORM; // how the buffer should be viewed
VkResult resultb = VK_SUCCESS;
VkSemaphore semaphorecmd;
VkFence fencecmd;
VkCommandBuffer cmdbufferforstaging;
VkCommandPool cmdpoolforstaging;
VkVertexInputBindingDescription* binding_descriptions =  NULL;
VkVertexInputAttributeDescription* attribute_descriptions = NULL;
VkPipelineStageFlags generating_stages = VK_PIPELINE_STAGE_TRANSFER_BIT;
VkPipelineStageFlags consuming_stgaes = VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
VkBufferMemoryBarrier *buffer_memory_barriers = NULL;
typedef struct
{ // like from this struct we will define how we will use this barries
    VkBuffer Buffer;
    VkAccessFlags CurrentAccess;
    VkAccessFlags NewAccess;
    uint32_t CurrentQueueFamily;
    uint32_t NewQueueFamily;
} BufferTransition;
typedef struct
{
    float position[2];
    float color[3];
} Vertex;

// Triangle vertices (NDC coordinates, so it shows up without any transforms)
static const Vertex triangle_vertices[] = {
    {{-0.5f,-0.5f} , {0.0f,1.0f,0.2f}},
    {{-0.5f, 0.5f} , {0.0f,0.0f,1.0f}},
    {{0.5f,0.5f},{1.0f,0.0f,0.0f}},
    {{0.5f,-0.5f},{1.0f,0.5f,0.0f}}
};
uint32_t index_buffer_data[] = {0, 1, 2, 2, 3, 0};
VkBufferCopy copyregion = {
    .srcOffset = 0,
    .dstOffset = 0,
    .size = sizeof(triangle_vertices)};
VkBufferCopy index_copyregion = {
    .srcOffset = 0,
    .dstOffset = 0,
    .size = sizeof(index_buffer_data)};
VkCommandBuffer *cmdbuffers = NULL;
VkMappedMemoryRange *memory_ranges = NULL;
VkMappedMemoryRange *memory_ranges_for_index = NULL;
BufferTransition *buffer_transitions = NULL;
VkDeviceSize offset = 0;                            // memory region that should be mapped and packed from cpu its our source which will be from cpu
VkDeviceSize data_size = sizeof(triangle_vertices); // size of data to be packed from cpu to the memory object of gp
VkDeviceSize index_data_size = sizeof(index_buffer_data);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool creatingcmdforstaging()
{
    uint32_t count = 1;
    VkCommandPoolCreateFlags parameters = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
    VkCommandPoolCreateInfo command_pool_create_info = {
        VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        NULL,
        parameters,
        graphics_queue_family_index

    };
    resultb = vkCreateCommandPool(logical_device, &command_pool_create_info, NULL, &cmdpoolforstaging);
    if (VK_SUCCESS != resultb)
    {
        printf("Couldn't create command pool\n");
        return false;
    }
    VkCommandBufferAllocateInfo command_buffer_allocate_info = {// allocating command buffer
                                                                VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                                                                NULL,
                                                                cmdpoolforstaging,
                                                                VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                                                                count};
    cmdbuffers = malloc(count * sizeof(VkCommandBuffer));
    resultb = vkAllocateCommandBuffers(logical_device, &command_buffer_allocate_info, cmdbuffers);
    if (VK_SUCCESS != resultb)
    {
        printf("couldn't create command buffer\n");
        return false;
    }
    cmdbufferforstaging = cmdbuffers[0]; // recording command buffer
    VkCommandBufferUsageFlags usage = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    VkCommandBufferInheritanceInfo *secondary_command_buffer_info = NULL;
    VkCommandBufferBeginInfo command_buffer_begin_info = {
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        NULL,
        usage,
        secondary_command_buffer_info};
    resultb = vkBeginCommandBuffer(cmdbufferforstaging, &command_buffer_begin_info);
    if (VK_SUCCESS != resultb)
    {
        printf("couldn't begin command buffer for staging\n");
        return false;
    }
    return true;
}
bool creatingbuffer()
{
    // VkDeviceSize size = sizeof(float);                             // see think of it in buffer sense just the size of data we want to store when creating a buffer
    // VkBufferUsageFlags usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT; // it says that this buffer can be used as source to shaders like can give values like vertex just dint care about other bits
    // VkBufferUsageFlags uniform_texel_usage = VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT;
    // VkBufferUsageFlags storage_texel_usage = VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT;
    // VkBufferCreateInfo buffer_create_info = {
    //   VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
    // nullptr,
    // 0,
    // size,
    // usage,
    // VK_SHARING_MODE_EXCLUSIVE, // its like whether queues from different family can access this buffer or not this bit says no and only one queue can access this buffer at a time
    // 0,
    //
    //  nullptr };
    /*VkBufferCreateInfo uniform_texel_buffer_create_info = {
        VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        nullptr,
        0,
        size,
        uniform_texel_usage,
        VK_SHARING_MODE_EXCLUSIVE, // its like whether queues from different family can access this buffer or not this bit says no and only one queue can access this buffer at a time
        0,
        nullptr };
    VkBufferCreateInfo storage_texel_buffer_create_info = {
        VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        nullptr,
        0,
        size,
        storage_texel_usage,
        VK_SHARING_MODE_EXCLUSIVE, // its like whether queues from different family can access this buffer or not this bit says no and only one queue can access this buffer at a time
        0,
        nullptr };

    resultb = vkCreateBuffer(logical_device, &buffer_create_info, nullptr, &buffer);
    if (VK_SUCCESS != resultb)
    {
        std::cout << "cant create buffer\n";
        return false;
    }
    resultb = vkCreateBuffer(logical_device, &uniform_texel_buffer_create_info, nullptr, &uniform_texel_buffer);
    if (VK_SUCCESS != resultb)
    {
        std::cout << "cant create  uniform texel buffer\n";
        return false;
    }
    resultb = vkCreateBuffer(logical_device, &storage_texel_buffer_create_info, nullptr, &storage_texel_buffer);
    if (VK_SUCCESS != resultb)
    {
        std::cout << "cant create  uniform texel buffer\n";
        return false;
    }*/
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // now let us do the same for staging buffer
    VkDeviceSize size_staging = sizeof(triangle_vertices);
    VkBufferUsageFlags usage_staging = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    VkBufferCreateInfo buffer_create_info_staging = {
        VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        NULL,
        0,
        size_staging,
        usage_staging,
        VK_SHARING_MODE_EXCLUSIVE,
        0,
        NULL

    };
    resultb = vkCreateBuffer(logical_device, &buffer_create_info_staging, NULL, &staging_buffer);
    if (VK_SUCCESS != resultb)
    {
        printf("cant create staging buffer\n");
        return false;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // now let us do the same for buffer which is going to accept the data from staging buffer
    VkDeviceSize size_dst_staging = sizeof(triangle_vertices);
    VkBufferUsageFlags usage_dst_staging = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    VkBufferCreateInfo buffer_create_info_dst_staging = {
        VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        NULL,
        0,
        size_dst_staging,
        usage_dst_staging,
        VK_SHARING_MODE_EXCLUSIVE,
        0,
        NULL};
    resultb = vkCreateBuffer(logical_device, &buffer_create_info_dst_staging, NULL, &staging_dst_buffer);
    if (VK_SUCCESS != resultb)
    {
        printf("cant create dst staging buffer\n");
        return false;
    }
    // now lets do the same buffer for index you foooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooool
    VkDeviceSize size_index_staging = sizeof(index_buffer_data);
    VkBufferUsageFlags usage_index_staging = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    VkBufferCreateInfo buffer_create_info_index_staging = {
        VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        NULL,
        0,
        size_index_staging,
        usage_index_staging,
        VK_SHARING_MODE_EXCLUSIVE,
        0,
        NULL};
    resultb = vkCreateBuffer(logical_device, &buffer_create_info_index_staging, NULL, &index_staging_buffer);
    if (VK_SUCCESS != resultb)
    {
        printf("cant create index staging buffer\n");
        return false;
    }
    // now let us do the same for buffer which is going to accept the data from staging buffer like from cpu to gpu
    VkDeviceSize size_index_dst_staging = sizeof(index_buffer_data);
    VkBufferUsageFlags usage_index_dst_staging = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    VkBufferCreateInfo buffer_create_info_index_dst_staging = {
        VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        NULL,
        0,
        size_index_dst_staging,
        usage_index_dst_staging,
        VK_SHARING_MODE_EXCLUSIVE,
        0,
        NULL};
    resultb = vkCreateBuffer(logical_device, &buffer_create_info_index_dst_staging, NULL, &index_buffer);
    if (VK_SUCCESS != resultb)
    {
        printf("cant create dindex dst staging buffer\n");
        return false;
    }
    return true;
}
bool allocatingandbindingmemory()
{
    vkGetPhysicalDeviceMemoryProperties(physical_device, &physical_device_memory_properties); // we are getting memory in physical device which the memory from which we can allocate memory for buffer like we are getting properties of physical device memory jut we will get size , number of heaps i g for staging here its no need
    // vkGetBufferMemoryRequirements(logical_device, buffer, &memory_requirements);                                 // we are getting requirements to assign memory to allocate memeory
    vkGetBufferMemoryRequirements(logical_device, staging_buffer, &memory_requirements_for_staging);         // now that last parameter will store the memory requiremnts
    vkGetBufferMemoryRequirements(logical_device, staging_dst_buffer, &memory_requirements_for_dst_staging); // now that last parameter will store the memory requiremnts
    vkGetBufferMemoryRequirements(logical_device, index_staging_buffer, &memory_requirements_for_index_staging);
    vkGetBufferMemoryRequirements(logical_device, index_buffer, &memory_requirements_for_index_dst);

    // vkGetBufferMemoryRequirements(logical_device, uniform_texel_buffer, &memory_requirements_for_uniform_texel); // now that last parameter will store the memory requiremnts
    //  here memory types we are getting properties of physical device
    //  now iterate over available physical Device memory types which are in memorytype count
    /*for (uint32_t type = 0; type < physical_device_memory_properties.memoryTypeCount; type++)
    { // here we took type 0 so every memory type count we will iterate we will loop untill all memory type in physical device count which is basically a array so we will iterate untill all memory type counts get included we will get type - 1 total count of types at last
        if ((memory_requirements.memoryTypeBits & (1 << type)) && ((physical_device_memory_properties.memoryTypes[type].propertyFlags & memory_properties) == memory_properties))
        {
            VkMemoryAllocateInfo buffer_memory_allocate_info = {
                VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                NULL,
                memory_requirements.size,
                type };
            resultb = vkAllocateMemory(logical_device, &buffer_memory_allocate_info, NULL, &memory_object); // now memory object is populated with buffer which is device local
            if (VK_SUCCESS == resultb)
            {
                break;
            }
        }
    }*/
    for (uint32_t type = 0; type < physical_device_memory_properties.memoryTypeCount; type++)
    { // here we took type 0 so every memory type count we will iterate we will loop untill all memory type in physical device count which is basically a array so we will iterate untill all memory type counts get included we will get type - 1 total count of types at last
        if ((memory_requirements_for_staging.memoryTypeBits & (1 << type)) && ((physical_device_memory_properties.memoryTypes[type].propertyFlags & memory_property_for_staging) == memory_property_for_staging))
        {
            // so memorytypebits are 32 bits on or off so we need to check when type is 0 we left shift one so left side gets 1 we do and if its getting one at that part its true
            VkMemoryAllocateInfo buffer_memory_allocate_info_for_staging = {
                VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                NULL,
                memory_requirements_for_staging.size,
                type};
            resultb = vkAllocateMemory(logical_device, &buffer_memory_allocate_info_for_staging, NULL, &memory_object_for_staging); // now memory object is populated with buffer which is host visible
            if (VK_SUCCESS == resultb)
            {
                break;
            }
        }
    }
    for (uint32_t type = 0; type < physical_device_memory_properties.memoryTypeCount; type++)
    { // here we took type 0 so every memory type count we will iterate we will loop untill all memory type in physical device count which is basically a array so we will iterate untill all memory type counts get included we will get type - 1 total count of types at last
        if ((memory_requirements_for_dst_staging.memoryTypeBits & (1 << type)) && ((physical_device_memory_properties.memoryTypes[type].propertyFlags & memory_property_for_dst_staging) == memory_property_for_dst_staging))
        {
            VkMemoryAllocateInfo buffer_memory_allocate_info_for_dst_staging = {
                VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                NULL,
                memory_requirements_for_dst_staging.size,
                type};
            resultb = vkAllocateMemory(logical_device, &buffer_memory_allocate_info_for_dst_staging, NULL, &memory_object_for_dst_staging); // now memory object is populated with buffer which is host visible
            if (VK_SUCCESS == resultb)
            {
                break;
            }
        }
    }
    for (uint32_t type = 0; type < physical_device_memory_properties.memoryTypeCount; type++)
    { // here we took type 0 so every memory type count we will iterate we will loop untill all memory type in physical device count which is basically a array so we will iterate untill all memory type counts get included we will get type - 1 total count of types at last
        if ((memory_requirements_for_index_staging.memoryTypeBits & (1 << type)) && ((physical_device_memory_properties.memoryTypes[type].propertyFlags & memory_property_for_index_staging) == memory_property_for_index_staging))
        {
            VkMemoryAllocateInfo buffer_memory_allocate_info_for_index_staging = {
                VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                NULL,
                memory_requirements_for_dst_staging.size,
                type};
            resultb = vkAllocateMemory(logical_device, &buffer_memory_allocate_info_for_index_staging, NULL, &memory_object_for_index_staging); // now memory object is populated with buffer which is host visible
            if (VK_SUCCESS == resultb)
            {
                break;
            }
        }
    }
    for (uint32_t type = 0; type < physical_device_memory_properties.memoryTypeCount; type++)
    { // here we took type 0 so every memory type count we will iterate we will loop untill all memory type in physical device count which is basically a array so we will iterate untill all memory type counts get included we will get type - 1 total count of types at last
        if ((memory_requirements_for_index_dst.memoryTypeBits & (1 << type)) && ((physical_device_memory_properties.memoryTypes[type].propertyFlags & memory_property_for_index_dst) == memory_property_for_index_dst))
        {
            VkMemoryAllocateInfo buffer_memory_allocate_info_for_index_dst = {
                VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                NULL,
                memory_requirements_for_index_dst.size,
                type};
            resultb = vkAllocateMemory(logical_device, &buffer_memory_allocate_info_for_index_dst, NULL, &memory_object_for_index_dst); // now memory object is populated with buffer which is host visible
            if (VK_SUCCESS == resultb)
            {
                break;
            }
        }
    }
    /*for (uint32_t type = 0; type < physical_device_memory_properties.memoryTypeCount; type++)
    { // here we took type 0 so every memory type count we will iterate we will loop untill all memory type in physical device count which is basically a array so we will iterate untill all memory type counts get included we will get type - 1 total count of types at last
        if ((memory_requirements_for_uniform_texel.memoryTypeBits & (1 << type)) && ((physical_device_memory_properties.memoryTypes[type].propertyFlags & memory_property_for_uniform_texel) == memory_property_for_uniform_texel))
        {
            VkMemoryAllocateInfo buffer_memory_allocate_info_for_uniform_texel = {
                VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                NULL,
                memory_requirements_for_uniform_texel.size,
                type };
            resultb = vkAllocateMemory(logical_device, &buffer_memory_allocate_info_for_uniform_texel, NULL, &memory_object_for_uniform_texel); // now memory object is populated with buffer which is host visible
            if (VK_SUCCESS == resultb)
            {
                break;
            }
        }
    }
    */
    /*for (uint32_t type = 0; type < physical_device_memory_properties.memoryTypeCount; type++)
    { // here we took type 0 so every memory type count we will iterate we will loop untill all memory type in physical device count which is basically a array so we will iterate untill all memory type counts get included we will get type - 1 total count of types at last
        if ((memory_requirements_for_storage_texel.memoryTypeBits & (1 << type)) && ((physical_device_memory_properties.memoryTypes[type].propertyFlags & memory_property_for_storage_texel) == memory_property_for_storage_texel))
        {
            VkMemoryAllocateInfo buffer_memory_allocate_info_for_storage_texel = {
                VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                NULL,
                memory_requirements_for_storage_texel.size,
                type };
            resultb = vkAllocateMemory(logical_device, &buffer_memory_allocate_info_for_storage_texel, NULL, &memory_object_for_storage_texel); // now memory object is populated with buffer which is host visible
            if (VK_SUCCESS == resultb)
            {
                break;
            }
        }
    }*/
    vkBindBufferMemory(logical_device, staging_buffer, memory_object_for_staging, 0);
    vkBindBufferMemory(logical_device, index_staging_buffer, memory_object_for_index_staging, 0);
    vkBindBufferMemory(logical_device, staging_dst_buffer, memory_object_for_dst_staging, 0);
    vkBindBufferMemory(logical_device, index_buffer, memory_object_for_index_dst, 0);
    return true;
}
// in my case see i have a vertex buffer its in staging then going to dst it smooth soo wiht this memory barrier there are many uses of buffer to be readby cmdbuffer like first i want to first it should br vertx
//  accepting then shader swew

// THIS IS MEMORY BARRIER FOR DESTINATION BUFFER/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// bool settingmemorybarrier()//buffers can be used as source of imformation which we can bind to pipeline and use shaders or we can store information from shaders
//{//there are many types of uses of a buffer we have to say that to driver so for this purpose there are memory buffer
// using namespace VulkanCookbook;
// return true;
//}
bool stagingresources() // i will just call it vertex buffer
{                       // here like i created memory in cpu but i cant just right directly on gpu so we staging buffers to mediate tranfer of data which are writte in cpu side to host visible side ig in integrted gpu its not required but ihave rtx 2050 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // its here acting as intermediate which will take dat afrom cpu to gpu
    const void *data = triangle_vertices;                                                                       // its the actual data which i want in staging buffer like it contains data to which i will give it host visin=ble buffer
    void *local_pointer;                                                                                        // this thing will have the memory address of  that data of stagingbuffer not at all related to gpu memopry
    resultb = vkMapMemory(logical_device, memory_object_for_staging, offset, VK_WHOLE_SIZE, 0, &local_pointer); // here i am getting the local_pointer a adrees of the dsta which is cpu it now have the pointer of host visble memory
    if (VK_SUCCESS != resultb)
    { // like now i want to share data so memoryobject contains all i want so i wiill use it now with map memory its
        printf("error in mapping memory\n");
        return false;
    }
    memcpy(local_pointer, data, data_size); // its saying like copy data like vertex then copy them in local_pointers pointing address
    memory_ranges = malloc(sizeof(VkMappedMemoryRange));
    memory_ranges[0] = (VkMappedMemoryRange){
        VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
        NULL,
        memory_object_for_staging,
        offset,
        VK_WHOLE_SIZE};
    resultb = vkFlushMappedMemoryRanges(logical_device, 1, &memory_ranges[0]);
    if (VK_SUCCESS != resultb)
    // like its saying i am done storing data in cpu i am ready to be fucked by gpu
    {
        printf("error in flushing mapping memory\n");
        return false;
    }
    vkUnmapMemory(logical_device, memory_object_for_staging);
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //now let us do same for index dst copying then sending it to dst (gpu) i will do it tomorow here youu fool -------------------------------------------------------------------------------------------------
    //------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    const void *data_index = index_buffer_data;                                                                       // its the actual data which i want in staging buffer like it contains data to which i will give it host visin=ble buffer 
    local_pointer; 
    resultb = vkMapMemory(logical_device,memory_object_for_index_staging ,offset, VK_WHOLE_SIZE, 0 , &local_pointer);
    if(VK_SUCCESS != resultb)
    {
        printf("error in mapping index memory\n");
        return false;
    }
    memcpy(local_pointer, data_index, index_data_size); 
    memory_ranges_for_index = malloc(sizeof(VkMappedMemoryRange));
    memory_ranges_for_index[0] = (VkMappedMemoryRange)
    {
        VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
        NULL,
        memory_object_for_index_staging,
        offset,
        VK_WHOLE_SIZE
    };
    resultb = vkFlushMappedMemoryRanges(logical_device,1,&memory_ranges_for_index[0]);
    if(resultb != VK_SUCCESS)
    {
        printf("error in flushing mapping memory\n");
        return false;   
    }
    vkUnmapMemory(logical_device,memory_object_for_index_staging);

    return true;
}
bool copyingvertexbuffer()
{
    // FROM HERE COPY INSTRUCTION CAN BE CALLED IN CMDBUFFER.CPP
    // here should be thw membarrier
    buffer_transitions = malloc(sizeof(BufferTransition) * (1) + 1);
    buffer_transitions[0] = (BufferTransition){staging_dst_buffer, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED}; // it is important to use () cause like in cpp we can ignore type but in c type cant be ignored
    // wait untill transfering is done then do change access to vertex read
    buffer_memory_barriers = malloc(sizeof(VkBufferMemoryBarrier) * 1);

    for (uint32_t i = 0; i < 1; i++)
    {
        buffer_memory_barriers[i] = (VkBufferMemoryBarrier){
            VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
            NULL,
            buffer_transitions[i].CurrentAccess,
            buffer_transitions[i].NewAccess,
            buffer_transitions[i].CurrentQueueFamily,
            buffer_transitions[i].NewQueueFamily,
            buffer_transitions[i].Buffer,
            0,
            VK_WHOLE_SIZE};
    }
    // NOW QUEUE SUBMIT IS IN CMDBUFFER AND IT WILL WAIT THERE ALSO ITS IN CMD BUFFER GO CHECK IT
    return true;}
// bool creatingbufferview()
//{//
//   using namespace VulkanCookbook;
//   VkBufferViewCreateInfo buffer_view_create_info = {
//     VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO,
//   nullptr,
// 0,
// buffer,
// format,
// memory_offset,
// memory_range };
// VkBufferView buffer_view; // now to store the view
// resultb = vkCreateBufferView(logical_device, &buffer_view_create_info, nullptr, &buffer_view);
// if (resultb != VK_SUCCESS)
//{
//   std::cout << "cant create buffer view\n";
//   return false;
//}
// else
//{
//    return true;
//}
/* VkBufferViewCreateInfo uniform_texel_buffer_view_create_info = {
     VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO,
     nullptr,
     0,
     uniform_texel_buffer,
     format,
     memory_offset,
     memory_range };
 VkBufferView uniform_texel_buffer_view; // now to store the view
 resultb = vkCreateBufferView(logical_device, &uniform_texel_buffer_view_create_info, nullptr, &uniform_texel_buffer_view);
 if (resultb != VK_SUCCESS)
 {
     std::cout << "cant create buffer view\n";
     return false;
 }
 else
 {
     return true;
 }
 VkBufferViewCreateInfo storage_texel_buffer_view_create_info = {
     VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO,
     nullptr,
     0,
     storage_texel_buffer,
     format,
     memory_offset,
     memory_range };
 VkBufferView storage_texel_buffer_view; // now to store the view
 resultb = vkCreateBufferView(logical_device, &storage_texel_buffer_view_create_info, nullptr, &storage_texel_buffer_view);
 if (resultb != VK_SUCCESS)
 {
     std::cout << "cant create buffer view\n";
     return false;
 }
 else
 {
     return true;
 }
 return true;
}*/
bool bufferbinding()//like to specify from where to take vertex data
{
    binding_descriptions = malloc(sizeof(VkVertexInputBindingDescription)*1);
    binding_descriptions[0] = (VkVertexInputBindingDescription){
        0,
        sizeof(Vertex),   //  was 8, now 3 cause i have 3 0.0f na that why or i can use the size of that vector aslo 
        VK_VERTEX_INPUT_RATE_VERTEX
    };
    attribute_descriptions = (VkVertexInputAttributeDescription*)malloc(sizeof(VkVertexInputAttributeDescription) * 2);
    memset(attribute_descriptions, 0, sizeof(VkVertexInputAttributeDescription) * 2);
    ///this is for vertex data 
    attribute_descriptions[0].binding = 0;
    attribute_descriptions[0].location = 0;
    attribute_descriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
    attribute_descriptions[0].offset = offsetof(Vertex, position);
    //now lets do for color woahhhh
    attribute_descriptions[1].binding = 0;
    attribute_descriptions[1].location = 1;
    attribute_descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attribute_descriptions[1].offset = offsetof(Vertex, color);

    /// i will bind vertex in pipeline.cpp cause my all bindings are from there
    return true;
}
void buffer_cleanup()
{
    vkFreeCommandBuffers(logical_device, cmdpoolforstaging, 1, &cmdbufferforstaging);
    vkDestroyCommandPool(logical_device, cmdpoolforstaging, NULL);
    vkDestroyBuffer(logical_device, staging_buffer, NULL);
    vkDestroyBuffer(logical_device, staging_dst_buffer, NULL);
    vkFreeMemory(logical_device, memory_object_for_staging, NULL);
    vkFreeMemory(logical_device, memory_object_for_dst_staging, NULL);
    free(cmdbuffers);
    free(memory_ranges);
    free(buffer_transitions);
    free(buffer_memory_barriers);
}