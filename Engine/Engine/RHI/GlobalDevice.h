#pragma once

#include <vulkan/vulkan.h>
#include "../Common/GraphicsEnums.h"
#include "../Vendor/vma/vk_mem_alloc.h"

#include <memory>

namespace RHI
{
	class Device;

	class GlobalDevice
	{
	public:
		GlobalDevice(const char* application_name, const char* engine_name);
		~GlobalDevice();

		inline const std::shared_ptr<Device> getDevice() const { return device; }

		void wait();

	private:
		std::shared_ptr<Device> device{ nullptr };
	};
}