#pragma once
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#include <string>
#include <vector>

#include "VulkanRendererContext.h"

#include "VulkanCubemapRenderer.h"
#include "VulkanTexture.h"
#include "VulkanShader.h"

namespace RHI
{
	class RenderScene;
	struct VulkanRenderFrame;
	struct UniformBufferObject;

	class Renderer
	{
	public:
		Renderer(const VulkanRendererContext& context, const VulkanSwapChainContext& swapChainContext)
			: context(context)
			, swapChainContext(swapChainContext)
			, hdriToCubeRenderer(context)
			, diffuseIrradianceRenderer(context)
			, environmentCubemap(context)
			, diffuseIrradianceCubemap(context)
		{
			
		}

		void init(const UniformBufferObject* ubo, const RenderScene* scene);
		void update(UniformBufferObject* ubo, const RenderScene* scene);
		void render(const UniformBufferObject* ubo, const RenderScene* scene, uint32_t imageIndex);
		void shutdown();

	private:
		void initEnvironment(const UniformBufferObject* ubo, const RenderScene* scene);
		void setEnvironment(const RenderScene* scene, int index);

	private:
		VulkanRendererContext context;
		VulkanSwapChainContext swapChainContext;

		VkRenderPass renderPass{ VK_NULL_HANDLE };
		VkDescriptorSetLayout descriptorSetLayout{ VK_NULL_HANDLE };
		VkPipelineLayout pipelineLayout{ VK_NULL_HANDLE };
		VkPipeline pbrPipeline{ VK_NULL_HANDLE };
		VkPipeline skyBoxPipeline{ VK_NULL_HANDLE };

		std::vector<VkFramebuffer> frameBuffers;
		std::vector<VkCommandBuffer> commandBuffers;

		std::vector<VkBuffer> uniformBuffers;
		std::vector<VkDeviceMemory> uniformBuffersMemory;

		std::vector<VkDescriptorSet> descriptorSets;

		// Cubemap
		VulkanTexture environmentCubemap;
		VulkanTexture diffuseIrradianceCubemap;
		// Cubemap Texture
		VulkanCubemapRenderer hdriToCubeRenderer;
		VulkanCubemapRenderer diffuseIrradianceRenderer;

		int currentEnvironment = 0;
	};
}