#pragma once

#include <vulkan/vulkan.h>
#include <string>
#include <vector>

#include "../Common/Mesh.h"
#include "../Common/Texture.h"
#include "Shader.h"

namespace RHI
{
	class VulkanContext;

	class Texture2DRenderer
	{
	public:
		Texture2DRenderer(const VulkanContext* context)
			: context(context), rendererQuad(context)
		{}

		void init();
		void shutdown();
		void render();

	private:
		const VulkanContext* context = nullptr;
		Mesh rendererQuad;
		VkExtent2D targetExtent;

		VkPipelineLayout pipelineLayout{ VK_NULL_HANDLE };
		VkRenderPass renderPass{ VK_NULL_HANDLE };
		VkPipeline pipeline{ VK_NULL_HANDLE };

		VkFramebuffer frameBuffer{ VK_NULL_HANDLE };
		VkCommandBuffer commandBuffer{ VK_NULL_HANDLE };
		VkFence fence{ VK_NULL_HANDLE };
	};
}
