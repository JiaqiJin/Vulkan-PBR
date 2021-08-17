#pragma once

#include <vulkan/vulkan.h>
#include <memory>

#include "DeviceObjectBase.h"

namespace Vulkan
{
	class BufferBase : public DeviceObjectBase
	{
	public:

		VkBuffer GetHandle() const { return m_buffer; }
		VkDeviceSize GetSize() const { return m_size; }
	protected:
		VkBuffer m_buffer{ VK_NULL_HANDLE };
		VkDeviceSize m_size{ 0 };
	};
}