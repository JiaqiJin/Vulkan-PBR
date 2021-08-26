#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <vector>

#include "ImageBase.h"
#include "Swapchain.h"

namespace Vulkan
{
	class SwapchainImage : public ImageBase
	{
	public:
		static std::vector<std::shared_ptr<SwapchainImage>> Create(const std::shared_ptr<Swapchain>& swapchain);

		const std::shared_ptr<Device>& GetDevicePtr() const override { return m_swapchain->GetDevicePtr(); };
		const std::shared_ptr<Swapchain>& GetSwapchain() const { return m_swapchain; }
	private:
		std::shared_ptr<Swapchain> m_swapchain;
	};
}