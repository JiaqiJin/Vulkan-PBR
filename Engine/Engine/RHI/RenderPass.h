#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace RHI
{
	class VulkanContext;

	// A render pass describes the scope of a rendering operation by specifying the collection of attachments, 
	// subpasses, and dependencies used during the rendering operation. Specify how many color buffer, depth buffer and sampler to use.
	class RenderPass
	{
	public:
		RenderPass() {}
		~RenderPass();

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

		VkRenderPass build(VkDevice device);

	private:
		struct SubpassData
		{
			std::vector<VkAttachmentReference> color_attachment_references;
			std::vector<VkAttachmentReference> color_attachment_resolve_references;
			VkAttachmentReference* depth_stencil_attachment_reference{ nullptr };
		};

		std::vector<VkAttachmentDescription> attachments;
		std::vector<VkSubpassDescription> subpass_infos;
		std::vector<SubpassData> subpass_datas;

		VkRenderPass renderPass{ VK_NULL_HANDLE };
	};
}