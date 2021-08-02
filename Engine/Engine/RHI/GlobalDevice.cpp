#include "GlobalDevice.h"
#include "Device.h"
#include <cassert>

namespace RHI
{
	GlobalDevice::GlobalDevice(const char* application_name, const char* engine_name)
	{
		device = new Device();
		device->init(application_name, engine_name);
	}

	GlobalDevice::~GlobalDevice()
	{
		if (device)
		{
			device->wait();
			device->shutdown();
		}

		delete device;
		device = nullptr;
	}
}