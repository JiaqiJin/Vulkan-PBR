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

	SemaphoreGroup::SemaphoreGroup(const std::initializer_list<std::shared_ptr<Semaphore>>& semaphores)
	{
		Initialize(semaphores);
	}

	SemaphoreGroup::SemaphoreGroup(const std::vector<std::shared_ptr<Semaphore>>& semaphores) { Initialize(semaphores); }

	void SemaphoreGroup::Initialize(const std::vector<std::shared_ptr<Semaphore>>& semaphores) 
	{
		m_semaphores.clear();
		for (const auto& i : semaphores)
			m_semaphores.push_back(i->GetHandle());
	}

	SemaphoreStageGroup::SemaphoreStageGroup(const std::initializer_list<std::pair<std::shared_ptr<Semaphore>, 
		VkPipelineStageFlags>>& stage_semaphores)
	{
		Initialize(stage_semaphores);
	}

	SemaphoreStageGroup::SemaphoreStageGroup(const std::vector<std::pair<std::shared_ptr<Semaphore>, VkPipelineStageFlags>>& stage_semaphores)
	{
		Initialize(stage_semaphores);
	}

	void SemaphoreStageGroup::Initialize(const std::vector<std::pair<std::shared_ptr<Semaphore>, VkPipelineStageFlags>>& stage_semaphores)
	{
		m_semaphores.clear();
		m_stages.clear();
		for (const auto& i : stage_semaphores)
		{
			m_semaphores.push_back(i.first->GetHandle());
			m_stages.push_back(i.second);
		}
	}
}