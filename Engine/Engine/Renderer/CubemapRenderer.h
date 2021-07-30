//#pragma once
//#define VK_USE_PLATFORM_WIN32_KHR
//#include <vulkan/vulkan.h>
//#include <string>
//#include <vector>
//
//#include "../RHI/Shader.h"
//#include "../Common/Texture.h"
//#include "../Common/Mesh.h"
//
//namespace RHI
//{
//	class VulkanContext;
//
//	class CubemapRenderer
//	{
//	public:
//		CubemapRenderer(const VulkanContext* context)
//			: context(context)
//			, rendererQuad(context)	{ }
//
//		void init(const Shader& vertexShader, const Shader& fragmentShader, const Texture& targetTexture);
//
//		void shutdown();
//
//		void render(const Texture& inputTexture);
//
//	private:
//		const VulkanContext* context{ nullptr };
//		Mesh rendererQuad; // Quad Mesh
//		VkExtent2D targetExtent; // Extend
//
//		// Cube Image faces Views
//		VkImageView faceViews[6]{ VK_NULL_HANDLE, VK_NULL_HANDLE, VK_NULL_HANDLE, VK_NULL_HANDLE, VK_NULL_HANDLE, VK_NULL_HANDLE };
//
//		VkPipelineLayout pipelineLayout{ VK_NULL_HANDLE }; // Pipeline layout
//		VkDescriptorSetLayout descriptorSetLayout{ VK_NULL_HANDLE }; // Descriptor set Layout
//		VkRenderPass renderPass{ VK_NULL_HANDLE }; // Render pass
//		VkPipeline pipeline{ VK_NULL_HANDLE }; // Pipeline
//
//		VkFramebuffer frameBuffer{ VK_NULL_HANDLE }; // Frame Buffer
//		VkCommandBuffer commandBuffer{ VK_NULL_HANDLE }; // Command Buffer
//		VkDescriptorSet descriptorSet{ VK_NULL_HANDLE }; // Descriptor set
//
//		VkBuffer uniformBuffer{ VK_NULL_HANDLE }; // Uniform Buffer
//		VkDeviceMemory uniformBufferMemory{ VK_NULL_HANDLE }; // Uniform buffer Memory
//
//		VkFence fence{ VK_NULL_HANDLE }; // Fence
//	};
//}