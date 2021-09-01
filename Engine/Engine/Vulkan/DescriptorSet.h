#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <vector>

#include "DeviceObjectBase.h"
#include "BufferBase.h"
#include "DescriptorPool.h"
#include "DescriptorSetLayout.h"
#include "DeviceObjectBase.h"
#include "ImageView.h"
#include "Sampler.h"

namespace Vulkan
{
	class DescriptorSet : public DeviceObjectBase
	{
	public:
		DescriptorSet(const std::shared_ptr<DescriptorPool>& descriptor_pool, const std::shared_ptr<DescriptorSetLayout>& descriptor_set_layout);
		~DescriptorSet();

		VkDescriptorSet GetHandle() const { return m_descriptor_set; }	
		const std::shared_ptr<Device>& GetDevicePtr() const override { return m_descriptor_pool->GetDevicePtr(); }
		const std::shared_ptr<DescriptorPool>& GetDescriptorPoolPtr() const { return m_descriptor_pool; }
		const std::shared_ptr<DescriptorSetLayout>& GetDescriptorSetLayoutPtr() const { return m_descriptor_set_layout;}

	private:
		std::shared_ptr<DescriptorPool> m_descriptor_pool;
		std::shared_ptr<DescriptorSetLayout> m_descriptor_set_layout;

		VkDescriptorSet m_descriptor_set{ VK_NULL_HANDLE };
	};
}