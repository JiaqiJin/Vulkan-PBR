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
