#include "RenderPass.h"
#include <stdexcept>

#include "../Common/Logger.h"

namespace RHI
{
	RenderPass::~RenderPass()
	{
		for (SubpassData& data : subpass_datas)
			delete data.depth_stencil_attachment_reference;
	}

	void RenderPass::addColorAttachment(VkFormat format,
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
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; // Images used as color attachment

		attachments.push_back(colorAttachment);
	}

	void RenderPass::addColorResolveAttachment(VkFormat format,
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
		colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;  // Images used as color attachment

		attachments.push_back(colorAttachmentResolve);
	}

	void RenderPass::addDepthStencilAttachment(VkFormat format,
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

	void RenderPass::addSubpass(VkPipelineBindPoint bindPoint)
	{
		VkSubpassDescription info = {};
		info.pipelineBindPoint = bindPoint;

		subpass_infos.push_back(info);
		subpass_datas.push_back(SubpassData());
	}

	void RenderPass::addColorAttachmentReference(int subpass, int attachment)
	{
		if (subpass < 0 || subpass >= subpass_infos.size()) { K_ERROR("addColorAttachmentReference"); return; }

		if (attachment < 0 || attachment >= attachments.size()) { K_ERROR("addColorAttachmentReference"); return; }

		VkAttachmentReference reference = {};
		reference.attachment = attachment;
		reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		SubpassData& data = subpass_datas[subpass];
		data.color_attachment_references.push_back(reference);
	}

	void RenderPass::addColorResolveAttachmentReference(int subpass, int attachment)
	{
		if (subpass < 0 || subpass >= subpass_infos.size()) { K_ERROR("addColorResolveAttachmentReference"); return; }

		if (attachment < 0 || attachment >= attachments.size()) { K_ERROR("addColorResolveAttachmentReference"); return; }

		VkAttachmentReference reference = {};
		reference.attachment = attachment;
		reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		SubpassData& data = subpass_datas[subpass];
		data.color_attachment_resolve_references.push_back(reference);

	}

	void RenderPass::setDepthStencilAttachmentReference(int subpass, int attachment)
	{
		if (subpass < 0 || subpass >= subpass_infos.size()){ K_ERROR("setDepthStencilAttachmentReference"); return; }
			
		if (attachment < 0 || attachment >= attachments.size()) { K_ERROR("setDepthStencilAttachmentReference"); return; }

		SubpassData& data = subpass_datas[subpass];

		if (data.depth_stencil_attachment_reference == nullptr)
			data.depth_stencil_attachment_reference = new VkAttachmentReference();

		VkAttachmentReference reference = {};
		reference.attachment = attachment;
		reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		*(data.depth_stencil_attachment_reference) = reference;
	}

	VkRenderPass RenderPass::build(VkDevice device)
	{
		for (int i = 0; i < subpass_infos.size(); i++)
		{
			SubpassData& data = subpass_datas[i];
			VkSubpassDescription& info = subpass_infos[i];

			info.pDepthStencilAttachment = data.depth_stencil_attachment_reference;
			info.colorAttachmentCount = static_cast<uint32_t>(data.color_attachment_references.size());
			info.pColorAttachments = data.color_attachment_references.data();
			info.pResolveAttachments = data.color_attachment_resolve_references.data();
		}

		VkRenderPassCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		info.attachmentCount = static_cast<uint32_t>(attachments.size());
		info.pAttachments = attachments.data();
		info.subpassCount = static_cast<uint32_t>(subpass_infos.size());
		info.pSubpasses = subpass_infos.data();

		VkRenderPass result = VK_NULL_HANDLE;
		if (vkCreateRenderPass(device, &info, nullptr, &result) != VK_SUCCESS)
		{
			K_ERROR("Can't create render pass");
		}

		return result;
	}
}