#pragma once
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#include <string>
#include <vector>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>

#include "VulkanRendererContext.h"

#include "VulkanCubemapRenderer.h"
#include "VulkanTexture.h"
#include "VulkanShader.h"

namespace RHI
{
	class RenderScene;

	struct UniformBufferObject
	{
		glm::mat4 world;
		glm::mat4 view;
		glm::mat4 proj;
		glm::vec3 cameraPosWS;
		float lerpUserValues{ 0.0f };
		float userMetalness{ 0.0f };
		float userRoughness{ 0.0f };
	};

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

		void init(const RenderScene* scene);
		void update(const RenderScene* scene);
		VkCommandBuffer render(const RenderScene* scene, uint32_t imageIndex);
		void shutdown();

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

		UniformBufferObject ubo;

		// Cubemap
		VulkanTexture environmentCubemap;
		VulkanTexture diffuseIrradianceCubemap;
		// Cubemap Texture
		VulkanCubemapRenderer hdriToCubeRenderer;
		VulkanCubemapRenderer diffuseIrradianceRenderer;
	};
}