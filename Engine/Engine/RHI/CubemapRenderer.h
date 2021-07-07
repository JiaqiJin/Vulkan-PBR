#pragma once
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#include <string>
#include <vector>

#include "RendererContext.h"
#include "Shader.h"
#include "../Common/Texture.h"
#include "../Common/Mesh.h"

namespace RHI
{
	class CubemapRenderer
	{
	public:
		CubemapRenderer(const RendererContext& context)
			: context(context)
			, rendererQuad(context)	{ }

		void init(const Shader& vertexShader, const Shader& fragmentShader, const Texture& targetTexture);

		void shutdown();

		void render(const Texture& inputTexture);

	private:
		RendererContext context;
		Mesh rendererQuad;
		VkExtent2D targetExtent;

		VkImageView faceViews[6]{ VK_NULL_HANDLE, VK_NULL_HANDLE, VK_NULL_HANDLE, VK_NULL_HANDLE, VK_NULL_HANDLE, VK_NULL_HANDLE };

		VkPipelineLayout pipelineLayout{ VK_NULL_HANDLE };
		VkDescriptorSetLayout descriptorSetLayout{ VK_NULL_HANDLE };
		VkRenderPass renderPass{ VK_NULL_HANDLE };
		VkPipeline pipeline{ VK_NULL_HANDLE };

		VkFramebuffer frameBuffer{ VK_NULL_HANDLE };
		VkCommandBuffer commandBuffer{ VK_NULL_HANDLE };
		VkDescriptorSet descriptorSet{ VK_NULL_HANDLE };

		VkBuffer uniformBuffer{ VK_NULL_HANDLE };
		VkDeviceMemory uniformBufferMemory{ VK_NULL_HANDLE };

		VkFence fence{ VK_NULL_HANDLE };
	};
}