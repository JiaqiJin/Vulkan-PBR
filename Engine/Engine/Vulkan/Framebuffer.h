#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <vector>

#include "DeviceObjectBase.h"
#include "RenderPass.h"

namespace Vulkan
{
	class ImageView;

	class Framebuffer : public DeviceObjectBase
	{
	public:
		Framebuffer(const std::shared_ptr<RenderPass>& render_pass,
			const std::shared_ptr<ImageView>& image_view, VkFramebufferCreateFlags flags = 0);
		~Framebuffer();

		VkFramebuffer GetHandle() const { return m_framebuffer; }
		const std::shared_ptr<Device>& GetDevicePtr() const override { return m_render_pass->GetDevicePtr(); }
		const std::shared_ptr<RenderPass>& GetRenderPass() const { return m_render_pass; }
		const std::vector<std::shared_ptr<ImageView>>& GetImageView() const { return m_image_view; }
		const VkExtent2D& GetExtent() const { return m_extent; }
		uint32_t GetLayers() const { return m_layers; }

	private:
		bool Init(const std::vector<std::shared_ptr<ImageView>>& image_views,
			const VkExtent2D& extent, uint32_t layers, VkFramebufferCreateFlags flags);
	private:
		std::shared_ptr<RenderPass> m_render_pass;
		std::vector<std::shared_ptr<ImageView>> m_image_view;

		VkFramebuffer m_framebuffer{ VK_NULL_HANDLE };

		VkExtent2D m_extent{};
		uint32_t m_layers{};
	};
}