#include "GenericGPUBuffer.h"

GenericGPUBuffer::GenericGPUBuffer()
    :
    mBuffer({})
{
}

GenericGPUBuffer::GenericGPUBuffer(GPUBuffer Buffer)
    :
    mBuffer(Buffer)
{
}

GenericGPUBuffer::~GenericGPUBuffer()
{
    mBuffer.Destory(Renderer::Get()->GetDevice());
}

GenericGPUBuffer GenericGPUBuffer::CreateOneTimeBuffer(VkDeviceSize Size, VkBufferUsageFlags Usage, VkMemoryPropertyFlags Properties)
{
    GPUBuffer buffer{};
    buffer.size = Size;

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = Size;
    bufferInfo.usage = Usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkDevice device = Renderer::Get()->GetDevice()->GetRaw();

    if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer.buffer) != VK_SUCCESS)
    {
        _THROW_RENDERER("Failed to create buffer!");
        return {};
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, buffer.buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = Renderer::Get()->GetPhysicalDevice()->FindMemoryType(memRequirements.memoryTypeBits, Properties);

    if (vkAllocateMemory(device, &allocInfo, nullptr, &buffer.memory) != VK_SUCCESS)
    {
        _THROW_RENDERER("Failed to allocate buffer memory!");
        return {};
    }

    vkBindBufferMemory(device, buffer.buffer, buffer.memory, 0);

    return GenericGPUBuffer(buffer);
}

GenericGPUBuffer* GenericGPUBuffer::Create(VkDeviceSize Size, VkBufferUsageFlags Usage, VkMemoryPropertyFlags Properties)
{
    GPUBuffer buffer{};
    buffer.size = Size;
    
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = Size;
    bufferInfo.usage = Usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkDevice device = Renderer::Get()->GetDevice()->GetRaw();

    if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer.buffer) != VK_SUCCESS) 
    {
        _THROW_RENDERER("Failed to create buffer!");
        return nullptr;
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, buffer.buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = Renderer::Get()->GetPhysicalDevice()->FindMemoryType(memRequirements.memoryTypeBits, Properties);
    
    if (vkAllocateMemory(device, &allocInfo, nullptr, &buffer.memory) != VK_SUCCESS)
    {
        _THROW_RENDERER("Failed to allocate buffer memory!");
        return nullptr;
    }

    vkBindBufferMemory(device, buffer.buffer, buffer.memory, 0);

    GenericGPUBuffer* genericBuffer = new GenericGPUBuffer();
    genericBuffer->mBuffer = buffer;
   
    return genericBuffer;
}

SharedPtr<GenericGPUBuffer> GenericGPUBuffer::CreateSingleFrameBuffer(VkDeviceSize Size, VkBufferUsageFlags Usage, VkMemoryPropertyFlags Properties)
{
    GPUBuffer buffer;
    buffer.size = Size;

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = Size;
    bufferInfo.usage = Usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkDevice device = Renderer::Get()->GetDevice()->GetRaw();

    if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer.buffer) != VK_SUCCESS)
    {
        _THROW_RENDERER("Failed to create buffer!");
        return nullptr;
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, buffer.buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = Renderer::Get()->GetPhysicalDevice()->FindMemoryType(memRequirements.memoryTypeBits, Properties);

    if (vkAllocateMemory(device, &allocInfo, nullptr, &buffer.memory) != VK_SUCCESS)
    {
        _THROW_RENDERER("Failed to allocate buffer memory!");
        return nullptr;
    }

    vkBindBufferMemory(device, buffer.buffer, buffer.memory, 0);

    SharedPtr<GenericGPUBuffer> genericBuffer = MakeShared<GenericGPUBuffer>();
    genericBuffer->mBuffer = buffer;

    Renderer::Get()->PushToCurrentFrameSyncGPUBuffer(genericBuffer);

    return genericBuffer;
}

void GenericGPUBuffer::Map(const void* Data, VkDeviceSize Size)
{
    VkDevice device = Renderer::Get()->GetDevice()->GetRaw();

    void* dst;
    vkMapMemory(device, mBuffer.memory, 0, Size, 0, &dst);
    memcpy(dst, Data, static_cast<size_t>(Size));
    vkUnmapMemory(device, mBuffer.memory);
}
