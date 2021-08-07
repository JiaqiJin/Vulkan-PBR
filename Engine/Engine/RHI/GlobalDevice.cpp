#include "GlobalDevice.h"
#include "Device.h"
#include <cassert>

#include "Framebuffer.h"
#include "RenderPass.h"

namespace RHI
{
	GlobalDevice::GlobalDevice(const char* application_name, const char* engine_name)
	{
		// Device
		device = std::make_shared<Device>();
		device->init(application_name, engine_name);
	}

	void GlobalDevice::wait()
	{
		vkDeviceWaitIdle(device->getDevice());
	}

	std::shared_ptr<FrameBuffer> GlobalDevice::createFramebuffer(const FrameBufferAttachment* attachments, uint8_t num_attachments)
	{
		std::shared_ptr<FrameBuffer> framebuffer = std::make_shared<FrameBuffer>(device);

		uint32_t width = 0;
		uint32_t height = 0;

		RenderPass renderPass;
		renderPass.addSubpass(VK_PIPELINE_BIND_POINT_GRAPHICS);

		framebuffer->num_attachments = 0;
		for (uint8_t i = 0; i < num_attachments; ++i)
		{
			const FrameBufferAttachment& attachment = attachments[i];
			VkImageView view = VK_NULL_HANDLE;
			VkFormat format = VK_FORMAT_UNDEFINED;
			VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;
			bool resolve = false;

			// add Color Attachment
			if (attachment.type == FrameBufferAttachmentType::COLOR)
			{
				//todo
			}

			// add Depth Attachment
			else if (attachment.type == FrameBufferAttachmentType::DEPTH)
			{
				//todo
			}

			// add Swap Chain Attachment
			else if (attachment.type == FrameBufferAttachmentType::SWAP_CHAIN_COLOR)
			{
				//todo
			}


		}

		return framebuffer;
	}

	GlobalDevice::~GlobalDevice()
	{
		if (device)
		{
			device->wait();
			device->shutdown();
		}
	}
}