#pragma once
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#include <string>
#include <vector>

#include "VulkanRendererContext.h"

#include "VulkanCubemapRenderer.h"
#include "../Common/Texture.h"
#include "VulkanShader.h"

namespace RHI
{
	class RenderScene;
	struct VulkanRenderFrame;
	struct UniformBufferObject;
	class SwapChain;

	class Renderer
	{
	public:
		Renderer(const VulkanRendererContext& context, VkExtent2D extent, VkDescriptorSetLayout descriptorSetLayout, VkRenderPass renderPass);

		virtual ~Renderer();

		void init(const UniformBufferObject* ubo, const RenderScene* scene);
		void update(UniformBufferObject* ubo, const RenderScene* scene);
		void resize(const SwapChain* swapChain);
		void render(const UniformBufferObject* ubo, const RenderScene* scene, const VulkanRenderFrame& frame);
		void shutdown();

	private:
		void initEnvironment(const UniformBufferObject* ubo, const RenderScene* scene);
		void setEnvironment(const RenderScene* scene, int index);

	private:
		VulkanRendererContext context;
		VkExtent2D extent;
		VkRenderPass renderPass{ VK_NULL_HANDLE };
		VkPipelineLayout pipelineLayout{ VK_NULL_HANDLE };

		//
		VulkanCubemapRenderer hdriToCubeRenderer;
		VulkanCubemapRenderer diffuseIrradianceRenderer;

		Texture environmentCubemap;
		Texture diffuseIrradianceCubemap;

		VkPipeline skyboxPipeline{ VK_NULL_HANDLE };
		VkPipeline pbrPipeline{ VK_NULL_HANDLE };

		VkDescriptorSetLayout descriptorSetLayout{ VK_NULL_HANDLE };
		VkDescriptorSetLayout sceneDescriptorSetLayout{ VK_NULL_HANDLE };
		VkDescriptorSet sceneDescriptorSet{ VK_NULL_HANDLE };

		uint32_t currentEnvironment{ 0 };
	};
}