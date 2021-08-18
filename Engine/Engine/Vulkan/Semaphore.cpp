#include "Semaphore.h"
#include "Device.h"
#include "../Common/Logger.h"

namespace Vulkan
{
	Semaphore::Semaphore(const std::shared_ptr<Device>& device)
		: m_device(device)
	{
		VkSemaphoreCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		if (vkCreateSemaphore(device->GetHandle(), &info, nullptr, &m_semaphore) != VK_SUCCESS)
			K_ERROR("Failed to create Semaphore");
	}

	Semaphore::~Semaphore()
	{
		if (m_semaphore)
			vkDestroySemaphore(m_device->GetHandle(), m_semaphore, nullptr);
	}
}