#pragma once
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#include <string>
#include <vector>

#include "CubemapRenderer.h"
#include "../Common/Texture.h"
#include "Shader.h"

namespace RHI
{
	class RenderScene;
	struct VulkanRenderFrame;
	class SwapChain;
	class VulkanContext;

	class Renderer
	{
	public:
		Renderer(const VulkanContext* context, VkExtent2D extent, VkDescriptorSetLayout descriptorSetLayout, VkRenderPass renderPass);

		virtual ~Renderer();

		void init(const RenderScene* scene);
		void resize(const SwapChain* swapChain);
		void render(const RenderScene* scene, const VulkanRenderFrame& frame);
		void shutdown();

		void reload(const RenderScene* scene);
		void setEnvironment(const Texture* texture);

	private:
		const VulkanContext* context{nullptr};
		VkExtent2D extent;
		VkRenderPass renderPass{ VK_NULL_HANDLE };
		VkPipelineLayout pipelineLayout{ VK_NULL_HANDLE };

		CubemapRenderer hdriToCubeRenderer;
		CubemapRenderer diffuseIrradianceRenderer;

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