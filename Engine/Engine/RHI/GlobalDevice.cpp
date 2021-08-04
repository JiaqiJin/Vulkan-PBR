#include "GlobalDevice.h"
#include "Device.h"
#include <cassert>

namespace RHI
{
	GlobalDevice::GlobalDevice(const char* application_name, const char* engine_name)
	{
		device = std::make_shared<Device>();
		device->init(application_name, engine_name);
	}

	void GlobalDevice::wait()
	{
		vkDeviceWaitIdle(device->getDevice());
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