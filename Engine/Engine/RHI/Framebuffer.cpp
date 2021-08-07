#include "Framebuffer.h"
#include "Device.h"
#include "../Common/Logger.h"

namespace RHI
{
	FrameBuffer::FrameBuffer(std::shared_ptr<Device> device) 
		: device(device)
	{
		
	}

	void FrameBuffer::Init(int width, int height, VkRenderPass renderPass, uint32_t num_attachments, const VkImageView* attachments)
	{
		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = num_attachments;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = width;
		framebufferInfo.height = height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(device->getDevice(), &framebufferInfo, nullptr, &framebuffer) != VK_SUCCESS)
		{
			K_ERROR("Failed create FrameBuffer");
		}
	}

}