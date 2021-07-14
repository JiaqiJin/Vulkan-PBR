#include "Texture2DRenderer.h"
#include "VulkanContext.h"
#include "GraphicsPipeline.h"
#include "PipelineLayout.h"
#include "DescriptorSetLayout.h"
#include "DescriptorSet.h"
#include "RenderPass.h"
#include "VulkanUtils.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>

namespace RHI
{
	void Texture2DRenderer::init()
	{
		// Init graphic pipeline TODO
	}

	void Texture2DRenderer::render()
	{
		// Record command buffer TODO
	}

	void Texture2DRenderer::shutdown()
	{
		vkDestroyFramebuffer(context->getDevice(), frameBuffer, nullptr);
		frameBuffer = VK_NULL_HANDLE;

		vkDestroyPipeline(context->getDevice(), pipeline, nullptr);
		pipeline = VK_NULL_HANDLE;

		vkDestroyPipelineLayout(context->getDevice(), pipelineLayout, nullptr);
		pipelineLayout = VK_NULL_HANDLE;

		vkDestroyRenderPass(context->getDevice(), renderPass, nullptr);
		renderPass = VK_NULL_HANDLE;

		vkFreeCommandBuffers(context->getDevice(), context->getCommandPool(), 1, &commandBuffer);
		commandBuffer = VK_NULL_HANDLE;

		vkDestroyFence(context->getDevice(), fence, nullptr);
		fence = VK_NULL_HANDLE;

		rendererQuad.clearGPUData();
		rendererQuad.clearCPUData();
	}
}