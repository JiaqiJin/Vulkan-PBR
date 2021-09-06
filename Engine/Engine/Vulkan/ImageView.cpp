#include "ImageView.h"
#include "Device.h"
#include "SwapchainImage.h"

#include "../Common/Logger.h"

namespace Vulkan
{
	ImageView::ImageView(const std::shared_ptr<ImageBase>& image, const VkImageViewCreateInfo& create_info)
	{
		VkImageViewCreateInfo new_info = create_info;
		new_info.image = image->GetHandle();

		if (vkCreateImageView(image->GetDevicePtr()->GetHandle(), &new_info, nullptr, &m_image_view) != VK_SUCCESS)
			K_ERROR("Failed to create Image View");
	}

	ImageView::ImageView(const std::shared_ptr<ImageBase>& image, VkImageViewType view_type,
		VkFormat format, VkImageAspectFlags aspect_mask, uint32_t base_mip_level,
		uint32_t level_count, uint32_t base_array_layer, uint32_t layer_count,
		const VkComponentMapping& components)
	{
		VkImageViewCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		create_info.image = image->GetHandle();
		create_info.viewType = view_type;
		create_info.format = format;
		create_info.subresourceRange.aspectMask = aspect_mask;
		create_info.subresourceRange.baseMipLevel = base_mip_level;
		create_info.subresourceRange.levelCount = level_count;
		create_info.subresourceRange.baseArrayLayer = base_array_layer;
		create_info.subresourceRange.layerCount = layer_count;
		create_info.components = components;

		if (vkCreateImageView(image->GetDevicePtr()->GetHandle(), &create_info, nullptr, &m_image_view) != VK_SUCCESS)
			K_ERROR("Failed to create Image View");
	}

	ImageView::ImageView(const std::shared_ptr<SwapchainImage>& swapchain_image)
	{
		VkImageViewCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		create_info.image = swapchain_image->GetHandle();
		create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		create_info.format = swapchain_image->GetSwapchain()->GetImageFormat();
		create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		create_info.subresourceRange.baseMipLevel = 0;
		create_info.subresourceRange.levelCount = 1;
		create_info.subresourceRange.baseArrayLayer = 0;
		create_info.subresourceRange.layerCount = 1;
		const VkComponentMapping& components = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY,
												   VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };
		create_info.components = components;

		if (vkCreateImageView(swapchain_image->GetDevicePtr()->GetHandle(), &create_info, nullptr, &m_image_view) != VK_SUCCESS)
			K_ERROR("Failed to create Image View");
	}

	ImageView::~ImageView()
	{
		if (m_image_view)
			vkDestroyImageView(m_image->GetDevicePtr()->GetHandle(), m_image_view, nullptr);
	}
}
