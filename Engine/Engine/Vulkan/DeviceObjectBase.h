#pragma once

#include "Device.h"

namespace Vulkan
{
	class DeviceObjectBase
	{
	public:
		virtual const std::shared_ptr<Device>& GetDevicePtr() const = 0;
	};
}