#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <vector>

#include "DeviceObjectBase.h"
#include "ImageBase.h"
#include "SwapchainImage.h"

namespace Vulkan
{
	class ImageView : public DeviceObjectBase
	{
	public:
		ImageView(const std::shared_ptr<ImageBase>& image, const VkImageViewCreateInfo& create_info);
		ImageView(const std::shared_ptr<ImageBase>& image, VkImageViewType view_type,
			VkFormat format, VkImageAspectFlags aspect_mask, uint32_t base_mip_level,
			uint32_t level_count, uint32_t base_array_layer, uint32_t layer_count,
			const VkComponentMapping& components);
		~ImageView();

		const std::shared_ptr<Device>& GetDevicePtr() const override { return m_image->GetDevicePtr(); };
		const std::shared_ptr<ImageBase>& GetImage() const { return m_image; };
		VkImageView GetHandle() const { return m_image_view; }

	private:
		std::shared_ptr<ImageBase> m_image;

		VkImageView m_image_view{ VK_NULL_HANDLE };
	};
}