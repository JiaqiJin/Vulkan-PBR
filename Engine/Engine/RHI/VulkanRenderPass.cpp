#include "VulkanRenderPass.h"
#include <stdexcept>

namespace RHI
{
	VulkanRenderPass::~VulkanRenderPass()
	{
		for (SubpassData& data : subpassDatas)
			delete data.depthStencilAttachmentReference;
	}

	void VulkanRenderPass::addColorAttachment(VkFormat format,
		VkSampleCountFlagBits msaaSamples,
		VkAttachmentLoadOp loadOp,
		VkAttachmentStoreOp storeOp,
		VkAttachmentLoadOp stencilLoadOp,
		VkAttachmentStoreOp stencilStoreOp)
	{
		VkAttachmentDescription colorAttachment = {};
		colorAttachment.format = format;
		colorAttachment.samples = msaaSamples;
		colorAttachment.loadOp = loadOp;
		colorAttachment.storeOp = storeOp;
		colorAttachment.stencilLoadOp = stencilLoadOp;
		colorAttachment.stencilStoreOp = stencilStoreOp;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		attachments.push_back(colorAttachment);
	}

	void VulkanRenderPass::addColorResolveAttachment(VkFormat format,
		VkAttachmentLoadOp loadOp,
		VkAttachmentStoreOp storeOp,
		VkAttachmentLoadOp stencilLoadOp,
		VkAttachmentStoreOp stencilStoreOp)
	{
		VkAttachmentDescription colorAttachmentResolve = {};
		colorAttachmentResolve.format = format;
		colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachmentResolve.loadOp = loadOp;
		colorAttachmentResolve.storeOp = storeOp;
		colorAttachmentResolve.stencilLoadOp = stencilLoadOp;
		colorAttachmentResolve.stencilStoreOp = stencilStoreOp;
		colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		attachments.push_back(colorAttachmentResolve);
	}

	void VulkanRenderPass::addDepthStencilAttachment(VkFormat format,
		VkSampleCountFlagBits msaaSamples,
		VkAttachmentLoadOp loadOp,
		VkAttachmentStoreOp storeOp,
		VkAttachmentLoadOp stencilLoadOp,
		VkAttachmentStoreOp stencilStoreOp)
	{
		VkAttachmentDescription depthAttachment = {};
		depthAttachment.format = format;
		depthAttachment.samples = msaaSamples;
		depthAttachment.loadOp = loadOp;
		depthAttachment.storeOp = storeOp;
		depthAttachment.stencilLoadOp = stencilLoadOp;
		depthAttachment.stencilStoreOp = stencilStoreOp;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		attachments.push_back(depthAttachment);
	}

	void VulkanRenderPass::addSubpass(VkPipelineBindPoint bindPoint)
	{
		VkSubpassDescription info = {};
		info.pipelineBindPoint = bindPoint;

		subpassInfos.push_back(info);
		subpassDatas.push_back(SubpassData());
	}

	void VulkanRenderPass::addColorAttachmentReference(int subpassIndex, int attachmentIndex)
	{
		if (subpassIndex < 0 || subpassIndex >= subpassInfos.size())
			return;

		if (attachmentIndex < 0 || attachmentIndex >= attachments.size())
			return;

		VkAttachmentReference reference = {};
		reference.attachment = attachmentIndex;
		reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		SubpassData& data = subpassDatas[subpassIndex];
		data.colorAttachmentReferences.push_back(reference);
	}

	void VulkanRenderPass::addColorResolveAttachmentReference(int subpassIndex, int attachmentIndex)
	{
		if (subpassIndex < 0 || subpassIndex >= subpassInfos.size())
			return;

		if (attachmentIndex < 0 || attachmentIndex >= attachments.size())
			return;

		VkAttachmentReference reference = {};
		reference.attachment = attachmentIndex;
		reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		SubpassData& data = subpassDatas[subpassIndex];
		data.colorAttachmentResolveReferences.push_back(reference);
	}

	void VulkanRenderPass::setDepthStencilAttachmentReference(int subpassIndex, int attachmentIndex)
	{
		if (subpassIndex < 0 || subpassIndex >= subpassInfos.size())
			return;

		if (attachmentIndex < 0 || attachmentIndex >= attachments.size())
			return;

		SubpassData& data = subpassDatas[subpassIndex];

		if (data.depthStencilAttachmentReference == nullptr)
			data.depthStencilAttachmentReference = new VkAttachmentReference();

		*(data.depthStencilAttachmentReference) = {};
		data.depthStencilAttachmentReference->attachment = attachmentIndex;
		data.depthStencilAttachmentReference->layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	}

	VkRenderPass VulkanRenderPass::build()
	{
		for (int i = 0; i < subpassInfos.size(); i++)
		{
			SubpassData& data = subpassDatas[i];
			VkSubpassDescription& info = subpassInfos[i];

			info.pDepthStencilAttachment = data.depthStencilAttachmentReference;
			info.colorAttachmentCount = static_cast<uint32_t>(data.colorAttachmentReferences.size());
			info.pColorAttachments = data.colorAttachmentReferences.data();
			info.pResolveAttachments = data.colorAttachmentResolveReferences.data();
		}

		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = static_cast<uint32_t>(subpassInfos.size());
		renderPassInfo.pSubpasses = subpassInfos.data();

		if (vkCreateRenderPass(context.device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
			throw std::runtime_error("Can't create render pass");

		return renderPass;
	}
}