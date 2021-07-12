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
		RenderPass(const VulkanContext* context)
			: context(context) { }

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

		VkRenderPass build();

	private:
		struct SubpassData
		{
			std::vector<VkAttachmentReference> colorAttachmentReferences; // color attachments
			std::vector<VkAttachmentReference> colorAttachmentResolveReferences;
			VkAttachmentReference* depthStencilAttachmentReference{ nullptr };
		};

		const VulkanContext* context{ nullptr };;

		std::vector<VkAttachmentDescription> attachments;
		std::vector<VkSubpassDescription> subpassInfos;
		std::vector<SubpassData> subpassDatas;

		VkRenderPass renderPass{ VK_NULL_HANDLE };
	};
}
