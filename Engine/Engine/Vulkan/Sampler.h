#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <vector>

#include "DeviceObjectBase.h"

namespace Vulkan
{
	class Sampler : public DeviceObjectBase
	{
	public:
		Sampler(const std::shared_ptr<Device>& device, const VkSamplerCreateInfo& create_info);
		Sampler(const std::shared_ptr<Device>& device, VkFilter filter,
			VkSamplerAddressMode address_mode,
			VkSamplerMipmapMode mipmap_mode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
			uint32_t mipmap_level = 1, bool request_anisotropy = false,
			float max_anisotropy = 1.0f);
		~Sampler();
		VkSampler GetHandle() const { return m_sampler; }
		const std::shared_ptr<Device>& GetDevicePtr() const override { return m_device; }

	private:
		std::shared_ptr<Device> m_device;
		VkSampler m_sampler{ VK_NULL_HANDLE };
	};
}