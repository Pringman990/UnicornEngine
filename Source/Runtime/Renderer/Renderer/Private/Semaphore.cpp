#include "Semaphore.h"

Semaphore::Semaphore()
    :
    mSemaphore(VK_NULL_HANDLE),
    mDevice(nullptr)
{
}

Semaphore::~Semaphore()
{
    vkDestroySemaphore(*mDevice, mSemaphore, nullptr);
    mSemaphore = VK_NULL_HANDLE;
    mDevice = nullptr;
}

Semaphore* Semaphore::Create(LogicalDevice* Device)
{
    Semaphore* semaphore = new Semaphore();
    semaphore->mDevice = Device;

    VkSemaphoreCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    if (vkCreateSemaphore(*Device, &createInfo, nullptr, &semaphore->mSemaphore) != VK_SUCCESS)
    {
        _THROW_RENDERER("Failed to create semaphore");
        return nullptr;
    }

    return semaphore;
}
