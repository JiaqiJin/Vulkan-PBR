#include "Framebuffer.h"
#include "Device.h"
#include "RenderPass.h"
#include "ImageView.h"

#include "../Common/Logger.h"

namespace Vulkan
{
	Framebuffer::Framebuffer(const std::shared_ptr<RenderPass>& render_pass,
		const std::shared_ptr<ImageView>& image_view, VkFramebufferCreateFlags flags)
		: m_render_pass(render_pass)
	{
		VkExtent3D extent_3d = image_view->GetImage()->GetExtent();
		if (!Init({ image_view }, { extent_3d.width, extent_3d.height }, image_view->GetImage()->GetArrayLayers(), flags))
			K_ERROR("Faile to create Framebuffer");
	}

	bool Framebuffer::Init(const std::vector<std::shared_ptr<ImageView>>& image_views,
		const VkExtent2D& extent, uint32_t layers, VkFramebufferCreateFlags flags)
	{
		m_image_view = image_views;
		m_extent = extent;
		m_layers = layers;

		std::vector<VkImageView> attachments(image_views.size());
		for (uint32_t i = 0; i < image_views.size(); ++i)
			attachments[i] = image_views[i]->GetHandle();

		VkFramebufferCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		create_info.flags = flags;
		create_info.renderPass = m_render_pass->GetHandle();
		create_info.attachmentCount = (uint32_t)attachments.size();
		create_info.pAttachments = attachments.data();
		create_info.width = extent.width;
		create_info.height = extent.height;
		create_info.layers = layers;

		if (vkCreateFramebuffer(GetDevicePtr()->GetHandle(), &create_info, nullptr, &m_framebuffer) != VK_SUCCESS)
			return false;

		return true;
	}

	Framebuffer::~Framebuffer()
	{
		if (m_framebuffer)
			vkDestroyFramebuffer(m_render_pass->GetDevicePtr()->GetHandle(), m_framebuffer, nullptr);
	}
}