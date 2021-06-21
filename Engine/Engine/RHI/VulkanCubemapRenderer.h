#pragma once
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#include <string>
#include <vector>

#include "VulkanRendererContext.h"
#include "VulkanShader.h"
#include "VulkanTexture.h"
#include "VulkanMesh.h"

namespace RHI
{
	class VulkanCubemapRenderer
	{
	public:
		VulkanCubemapRenderer(const VulkanRendererContext& context)
			: context(context)
			, rendererQuad(context)	{ }

		void init(const VulkanShader& vertexShader, const VulkanShader& fragmentShader,
			const VulkanTexture& inputTexture, const VulkanTexture& targetTexture);

		void shutdown();

		void render();

	private:
		VulkanRendererContext context;
		VulkanMesh rendererQuad;

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
	};
}