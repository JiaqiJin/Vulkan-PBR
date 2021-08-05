#pragma once

#include <vulkan/vulkan.h>

#include "../Common/GraphicsEnums.h"

#define MAX_ATTACHMENTS 16

namespace RHI
{
	class FrameBuffer
	{
	public:
		FrameBuffer(const FrameBufferAttachment* attachments);
		~FrameBuffer();

	private:
		VkFramebuffer framebuffer{ VK_NULL_HANDLE };
		VkExtent2D sizes{ 0, 0 };

		VkRenderPass dummy_render_pass{ VK_NULL_HANDLE }; // TODO: move to render pass cache

		uint8_t num_attachments{ 0 };
		VkImageView attachments[MAX_ATTACHMENTS];
		FrameBufferAttachmentType attachment_types[MAX_ATTACHMENTS];
		VkFormat attachment_formats[MAX_ATTACHMENTS];
		VkSampleCountFlagBits attachment_samples[MAX_ATTACHMENTS];
		bool attachment_resolve[MAX_ATTACHMENTS];
	};
}