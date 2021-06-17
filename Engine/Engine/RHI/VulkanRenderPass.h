#pragma once

#include <vulkan/vulkan.h>
#include <vector>

#include "VulkanRendererContext.h"

namespace RHI
{
	class VulkanRenderPass
	{
	public:
		VulkanRenderPass(const VulkanRendererContext& context)
			: context(context) { }
		~VulkanRenderPass();

		inline VkRenderPass getRenderPass() const { return renderPass; }

		void addColorAttachment(VkFormat format, VkSampleCountFlagBits msaaSamples);

		void addColorResolveAttachment(VkFormat format);

		void addDepthStencilAttachment(VkFormat format, VkSampleCountFlagBits msaaSamples);

		void addSubpass(VkPipelineBindPoint bindPoint);

		void addColorAttachmentReference(int subpassIndex, int attachmentIndex);

		void addColorResolveAttachmentReference(int subpassIndex, int attachmentIndex);

		void setDepthStencilAttachmentReference(int subpassIndex, int attachmentIndex);

		VkRenderPass build();

	private:
		struct SubpassData
		{
			std::vector<VkAttachmentReference> colorAttachmentReferences;
			std::vector<VkAttachmentReference> colorAttachmentResolveReferences;
			VkAttachmentReference* depthStencilAttachmentReference{ nullptr };
		};

		VulkanRendererContext context;

		std::vector<VkAttachmentDescription> attachments;
		std::vector<VkSubpassDescription> subpassInfos;
		std::vector<SubpassData> subpassDatas;

		VkRenderPass renderPass{ VK_NULL_HANDLE };
	};
}

/*
	// Create render pass
		VulkanRenderPass renderPassBuild(context);
		renderPassBuild.addColorAttachment(swapChainContext.colorFormat, context.msaaSamples);
		renderPassBuild.addColorResolveAttachment(swapChainContext.colorFormat);
		renderPassBuild.addDepthStencilAttachment(swapChainContext.depthFormat, context.msaaSamples);
		renderPassBuild.addSubpass(VK_PIPELINE_BIND_POINT_GRAPHICS);
		renderPassBuild.addColorAttachmentReference(0, 0);
		renderPassBuild.addColorResolveAttachmentReference(0, 1);
		renderPassBuild.setDepthStencilAttachmentReference(0, 2);

		renderPass = renderPassBuild.build();
*/
