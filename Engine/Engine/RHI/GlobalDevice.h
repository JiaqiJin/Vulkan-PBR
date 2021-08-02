#pragma once

#include <vulkan/vulkan.h>
#include "../Common/GraphicsEnums.h"
#include "../Vendor/vma/vk_mem_alloc.h"

namespace RHI
{
	class Device;

	class GlobalDevice
	{
	public:
		GlobalDevice(const char* application_name, const char* engine_name);
		~GlobalDevice();

		inline const Device* getDevice() const { return device; }

		void wait();

	private:
		Device* device{ nullptr };
	};
}