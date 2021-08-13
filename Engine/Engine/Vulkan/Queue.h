#pragma once

#include <vulkan/vulkan.h>
#include <cinttypes>
#include <memory>
#include <mutex>

#include "DeviceObjectBase.h"

namespace Vulkan
{
	class UniqueQueue : public DeviceObjectBase
	{
	public:
		static std::shared_ptr<UniqueQueue> Create(const std::shared_ptr<Device>& device, uint32_t family_index,
			uint32_t queue_index);

		const std::shared_ptr<Device>& GetDevicePtr() const override { return m_device; }

	private:
		std::shared_ptr<Device> m_device;
		VkQueue m_queue{ VK_NULL_HANDLE };
		uint32_t m_family_index;
		std::mutex m_mutex;

		friend class Queue;
		friend class PresentQueue;
	};

	class Queue : public DeviceObjectBase 
	{ // only can be created with device creation

	public:
		static std::shared_ptr<Queue> Create(const std::shared_ptr<UniqueQueue>& unique_queue);

		const std::shared_ptr<Device>& GetDevicePtr() const override { return m_unique_queue->GetDevicePtr(); }

		VkQueue GetHandle() const { return m_unique_queue->m_queue; }

		uint32_t GetFamilyIndex() const { return m_unique_queue->m_family_index; }

		std::mutex& GetMutex() const { return m_unique_queue->m_mutex; }

		VkResult WaitIdle() const;
	protected:
		std::shared_ptr<UniqueQueue> m_unique_queue;
	};

	class Surface;

	class PresentQueue : public Queue // only can be created with device creation
	{ 
	public:
		static std::shared_ptr<PresentQueue> Create(const std::shared_ptr<UniqueQueue>& unique_queue,
			const std::shared_ptr<Surface>& surface);

		const std::shared_ptr<Surface>& GetSurfacePtr() const { return m_surface; }
	private:
		std::shared_ptr<Surface> m_surface;

	};
}