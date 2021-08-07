#pragma once

#include <vulkan/vulkan.h>
#include <memory>

#include "../Common/GraphicsEnums.h"

#define MAX_ATTACHMENTS 16

namespace RHI
{
	class Device;

	class FrameBuffer
	{
	public:
		FrameBuffer(std::shared_ptr<Device> device);

		void Init(int width, int height, VkRenderPass renderPass, uint32_t num_attachments, const VkImageView* attachments);

		uint8_t num_attachments{ 0 };

	private:
		std::shared_ptr<Device> device;
		VkFramebuffer framebuffer{ VK_NULL_HANDLE };

		VkImageView attachments[MAX_ATTACHMENTS];
		VkFormat attachment_formats[MAX_ATTACHMENTS];
		VkSampleCountFlagBits attachment_samples[MAX_ATTACHMENTS];
		bool attachment_resolve[MAX_ATTACHMENTS];

	};
}