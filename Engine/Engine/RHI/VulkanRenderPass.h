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

		void addColorAttachment(VkFormat format,
			VkSampleCountFlagBits msaaSamples,
			VkAttachmentLoadOp loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			VkAttachmentStoreOp storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
			VkAttachmentLoadOp stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			VkAttachmentStoreOp stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE);

		void addColorResolveAttachment(VkFormat format,
			VkAttachmentLoadOp loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			VkAttachmentStoreOp storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
			VkAttachmentLoadOp stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			VkAttachmentStoreOp stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE);

		void addDepthStencilAttachment(VkFormat format,
			VkSampleCountFlagBits msaaSamples,
			VkAttachmentLoadOp loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			VkAttachmentStoreOp storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
			VkAttachmentLoadOp stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			VkAttachmentStoreOp stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE);

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
