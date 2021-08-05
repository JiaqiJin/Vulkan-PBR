#include "GlobalDevice.h"
#include "Device.h"
#include <cassert>

#include "Framebuffer.h"

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

	std::shared_ptr<FrameBuffer> GlobalDevice::createFramebuffer(const FrameBufferAttachment* attachments)
	{
		std::shared_ptr<FrameBuffer> framebuffer = std::make_shared<FrameBuffer>(attachments);

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