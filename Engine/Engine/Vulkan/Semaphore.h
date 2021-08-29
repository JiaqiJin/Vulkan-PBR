#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <vector>

#include "DeviceObjectBase.h"

namespace Vulkan
{
	class Semaphore : public DeviceObjectBase
	{
	public:
		Semaphore(const std::shared_ptr<Device>& device);
		~Semaphore();

		VkSemaphore GetHandle() const { return m_semaphore; }
		const std::shared_ptr<Device>& GetDevicePtr() const override { return m_device; }
	private:
		std::shared_ptr<Device> m_device;

		VkSemaphore m_semaphore{ VK_NULL_HANDLE };
	};

	class SemaphoreGroup 
	{
	public:
		SemaphoreGroup() = default;

		SemaphoreGroup(const std::initializer_list<std::shared_ptr<Semaphore>>& semaphores);

		explicit SemaphoreGroup(const std::vector<std::shared_ptr<Semaphore>>& semaphores);

		void Initialize(const std::vector<std::shared_ptr<Semaphore>>& semaphores);

		uint32_t GetCount() const { return m_semaphores.size(); }

		const VkSemaphore* GetSemaphoresPtr() const { return m_semaphores.data(); }
	private:
		std::vector<VkSemaphore> m_semaphores;

	};

	class SemaphoreStageGroup 
	{
	public:
		SemaphoreStageGroup() = default;

		SemaphoreStageGroup(const std::initializer_list<std::pair<std::shared_ptr<Semaphore>, VkPipelineStageFlags>>& stage_semaphores);

		explicit SemaphoreStageGroup(const std::vector<std::pair<std::shared_ptr<Semaphore>, VkPipelineStageFlags>>& stage_semaphores);

		void Initialize(const std::vector<std::pair<std::shared_ptr<Semaphore>, VkPipelineStageFlags>>& stage_semaphores);

		uint32_t GetCount() const { return m_semaphores.size(); }
		const VkSemaphore* GetSemaphoresPtr() const { return m_semaphores.data(); }
		const VkPipelineStageFlags* GetWaitStagesPtr() const { return m_stages.data(); }
	private:
		std::vector<VkSemaphore> m_semaphores;
		std::vector<VkPipelineStageFlags> m_stages;

	};
}