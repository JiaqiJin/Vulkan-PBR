#include "VulkanRenderer.h"	
#include "VulkanMesh.h"
#include "VulkanGraphicsPipeline.h"
#include "VulkanPipelineLayout.h"
#include "VulkanDescriptorSetLayout.h"
#include "VulkanDescriptorSet.h"
#include "VulkanRenderPass.h"
#include "VulkanUtils.h"
#include "VulkanApplication.h"

#include "RenderScene.h"

#include "../Vendor/imgui/imgui.h"
#include "../Vendor/imgui/imgui_impl_vulkan.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>

#include <chrono>	

namespace RHI
{

	void Renderer::init(const UniformBufferObject* ubo, const RenderScene* scene)
	{
		const VulkanShader* pbrVertexShader = scene->getPBRVertexShader();
		const VulkanShader* pbrFragmentShader = scene->getPBRFragmentShader();
		const VulkanShader* skyboxVertexShader = scene->getSkyboxVertexShader();
		const VulkanShader* skyboxFragmentShader = scene->getSkyboxFragmentShader();

		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(swapChainContext.extent.width);
		viewport.height = static_cast<float>(swapChainContext.extent.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor = {};
		scissor.offset = { 0, 0 };
		scissor.extent = swapChainContext.extent;

		VkShaderStageFlags stage = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

		VulkanDescriptorSetLayout descriptorSetLayoutBuilder(context);
		
		descriptorSetLayoutBuilder.addDescriptorBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, stage);
		descriptorSetLayoutBuilder.addDescriptorBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, stage);
		descriptorSetLayoutBuilder.addDescriptorBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, stage);
		descriptorSetLayoutBuilder.addDescriptorBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, stage);
		descriptorSetLayoutBuilder.addDescriptorBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, stage);
		descriptorSetLayoutBuilder.addDescriptorBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, stage);
		descriptorSetLayoutBuilder.addDescriptorBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, stage);
		descriptorSetLayoutBuilder.addDescriptorBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, stage);
		descriptorSetLayout = descriptorSetLayoutBuilder.build();

		VulkanRenderPass renderPassBuilder(context);
		renderPassBuilder.addColorAttachment(swapChainContext.colorFormat, context.msaaSamples);
		renderPassBuilder.addColorResolveAttachment(swapChainContext.colorFormat);
		renderPassBuilder.addDepthStencilAttachment(swapChainContext.depthFormat, context.msaaSamples);
		renderPassBuilder.addSubpass(VK_PIPELINE_BIND_POINT_GRAPHICS);
		renderPassBuilder.addColorAttachmentReference(0, 0);
		renderPassBuilder.addColorResolveAttachmentReference(0, 1);
		renderPassBuilder.setDepthStencilAttachmentReference(0, 2);
		renderPass = renderPassBuilder.build();

		VulkanPipelineLayout pipelineLayoutBuilder(context);
		pipelineLayoutBuilder.addDescriptorSetLayout(descriptorSetLayout);
		pipelineLayout = pipelineLayoutBuilder.build();

		VulkanGraphicsPipeline pbrPipelineBuilder(context, pipelineLayout, renderPass);
		
		pbrPipelineBuilder.addShaderStage(pbrVertexShader->getShaderModule(), VK_SHADER_STAGE_VERTEX_BIT);
		pbrPipelineBuilder.addShaderStage(pbrFragmentShader->getShaderModule(), VK_SHADER_STAGE_FRAGMENT_BIT);
		pbrPipelineBuilder.addVertexInput(VulkanMesh::getVertexInputBindingDescription(), VulkanMesh::getAttributeDescriptions());
		pbrPipelineBuilder.setInputAssemblyState(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
		pbrPipelineBuilder.addViewport(viewport);
		pbrPipelineBuilder.addScissor(scissor);
		pbrPipelineBuilder.setRasterizerState(false, false, VK_POLYGON_MODE_FILL, 1.0f, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE);
		pbrPipelineBuilder.setMultisampleState(context.msaaSamples, true);
		pbrPipelineBuilder.setDepthStencilState(true, true, VK_COMPARE_OP_LESS);
		pbrPipelineBuilder.addBlendColorAttachment();
		pbrPipeline = pbrPipelineBuilder.build();

		VulkanGraphicsPipeline skyboxPipelineBuilder(context, pipelineLayout, renderPass);
		
		skyboxPipelineBuilder.addShaderStage(skyboxVertexShader->getShaderModule(), VK_SHADER_STAGE_VERTEX_BIT);
		skyboxPipelineBuilder.addShaderStage(skyboxFragmentShader->getShaderModule(), VK_SHADER_STAGE_FRAGMENT_BIT);
		skyboxPipelineBuilder.addVertexInput(VulkanMesh::getVertexInputBindingDescription(), VulkanMesh::getAttributeDescriptions());
		skyboxPipelineBuilder.setInputAssemblyState(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
		skyboxPipelineBuilder.addViewport(viewport);
		skyboxPipelineBuilder.addScissor(scissor);
		skyboxPipelineBuilder.setRasterizerState(false, false, VK_POLYGON_MODE_FILL, 1.0f, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE);
		skyboxPipelineBuilder.setMultisampleState(context.msaaSamples, true);
		skyboxPipelineBuilder.setDepthStencilState(true, true, VK_COMPARE_OP_LESS);
		skyboxPipelineBuilder.addBlendColorAttachment();
		skyBoxPipeline = skyboxPipelineBuilder.build();

		// Create uniform buffers
		VkDeviceSize uboSize = sizeof(ubo);

		uint32_t imageCount = static_cast<uint32_t>(swapChainContext.swapChainImageViews.size());
		uniformBuffers.resize(imageCount);
		uniformBuffersMemory.resize(imageCount);

		for (uint32_t i = 0; i < imageCount; i++)
		{
			VulkanUtils::createBuffer(
				context,
				uboSize,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				uniformBuffers[i],
				uniformBuffersMemory[i]
			);
		}

		// Create descriptor sets
		std::vector<VkDescriptorSetLayout> layouts(imageCount, descriptorSetLayout);

		VkDescriptorSetAllocateInfo descriptorSetAllocInfo = {};
		descriptorSetAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descriptorSetAllocInfo.descriptorPool = context.descriptorPool;
		descriptorSetAllocInfo.descriptorSetCount = imageCount;
		descriptorSetAllocInfo.pSetLayouts = layouts.data();

		descriptorSets.resize(imageCount);
		if (vkAllocateDescriptorSets(context.device, &descriptorSetAllocInfo, descriptorSets.data()) != VK_SUCCESS)
			throw std::runtime_error("Can't allocate descriptor sets");

		// Init Cubemap
		initEnvironment(ubo, scene);

		for (size_t i = 0; i < imageCount; i++)
		{
			std::array<const VulkanTexture*, 7> textures =
			{
				scene->getAlbedoTexture(),
				scene->getNormalTexture(),
				scene->getAOTexture(),
				scene->getShadingTexture(),
				scene->getEmissionTexture(),
				& environmentCubemap,
				& diffuseIrradianceCubemap,
			};

			VulkanUtils::bindUniformBuffer(
				context,
				descriptorSets[i],
				0,
				uniformBuffers[i],
				0,
				sizeof(ubo)
			);

			for (int k = 0; k < textures.size(); k++)
				VulkanUtils::bindCombinedImageSampler(
					context,
					descriptorSets[i],
					k + 1,
					textures[k]->getImageView(),
					textures[k]->getSampler());
		}

		// Create framebuffers
		frameBuffers.resize(imageCount);
		for (size_t i = 0; i < imageCount; i++) {
			std::array<VkImageView, 3> attachments = {
				swapChainContext.colorImageView,
				swapChainContext.swapChainImageViews[i],
				swapChainContext.depthImageView,
			};

			VkFramebufferCreateInfo framebufferInfo = {};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = renderPass;
			framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebufferInfo.pAttachments = attachments.data();
			framebufferInfo.width = swapChainContext.extent.width;
			framebufferInfo.height = swapChainContext.extent.height;
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer(context.device, &framebufferInfo, nullptr, &frameBuffers[i]) != VK_SUCCESS)
				throw std::runtime_error("Can't create framebuffer");
		}

		// Create command buffers
		commandBuffers.resize(imageCount);

		VkCommandBufferAllocateInfo allocateInfo = {};
		allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocateInfo.commandPool = context.commandPool;
		allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocateInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(context.device, &allocateInfo, commandBuffers.data()) != VK_SUCCESS)
			throw std::runtime_error("Can't create command buffers");

		// Init ImGui bindings for Vulkan
		{
			ImGui_ImplVulkan_InitInfo init_info = {};
			init_info.Instance = context.instance;
			init_info.PhysicalDevice = context.physicalDevice;
			init_info.Device = context.device;
			init_info.QueueFamily = context.graphicsQueueFamily;
			init_info.Queue = context.graphicsQueue;
			init_info.DescriptorPool = context.descriptorPool;
			init_info.MSAASamples = context.msaaSamples;
			init_info.MinImageCount = static_cast<uint32_t>(swapChainContext.swapChainImageViews.size());
			init_info.ImageCount = static_cast<uint32_t>(swapChainContext.swapChainImageViews.size());

			ImGui_ImplVulkan_Init(&init_info, renderPass);

			VulkanRendererContext imGuiContext = {};
			imGuiContext.commandPool = context.commandPool;
			imGuiContext.descriptorPool = context.descriptorPool;
			imGuiContext.device = context.device;
			imGuiContext.graphicsQueue = context.graphicsQueue;
			imGuiContext.msaaSamples = context.msaaSamples;
			imGuiContext.physicalDevice = context.physicalDevice;
			imGuiContext.presentQueue = context.presentQueue;

			VkCommandBuffer imGuiCommandBuffer = VulkanUtils::beginSingleTimeCommands(imGuiContext);
			ImGui_ImplVulkan_CreateFontsTexture(imGuiCommandBuffer);
			VulkanUtils::endSingleTimeCommands(imGuiContext, imGuiCommandBuffer);
		}
	}

	void Renderer::initEnvironment(const UniformBufferObject* ubo, const RenderScene* scene)
	{
		// Irradiance pre-computed sum of all indirect diffuse light of the scene hitting some surface aligned along direction wo
		// The irradiance map displays somewhat like an average color or lighting display of the environment
		environmentCubemap.createCube(VK_FORMAT_R32G32B32A32_SFLOAT, 256, 256, 1);
		diffuseIrradianceCubemap.createCube(VK_FORMAT_R32G32B32A32_SFLOAT, 256, 256, 1);

		hdriToCubeRenderer.init(
			*scene->getCubeVertexShader(),
			*scene->getHDRIToFragmentShader(),
			environmentCubemap);

		diffuseIrradianceRenderer.init(
			*scene->getCubeVertexShader(),
			*scene->getDiffuseIrradianceFragmentShader(),
			diffuseIrradianceCubemap);

		setEnvironment(scene, currentEnvironment);;
	}

	void Renderer::setEnvironment(const RenderScene* scene, int index)
	{
		{
			VulkanUtils::transitionImageLayout(
				context,
				environmentCubemap.getImage(),
				environmentCubemap.getImageFormat(),
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
				0, environmentCubemap.getNumMipLevels(),
				0, environmentCubemap.getNumLayers());

			hdriToCubeRenderer.render(*scene->getHDRTexture(index));
			
			VulkanUtils::transitionImageLayout(
				context,
				environmentCubemap.getImage(),
				environmentCubemap.getImageFormat(),
				VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				0, environmentCubemap.getNumMipLevels(),
				0, environmentCubemap.getNumLayers());
		}

		// Diffuse irradiance images transitions
		{
			VulkanUtils::transitionImageLayout(
				context,
				diffuseIrradianceCubemap.getImage(),
				diffuseIrradianceCubemap.getImageFormat(),
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
				0, diffuseIrradianceCubemap.getNumMipLevels(),
				0, diffuseIrradianceCubemap.getNumLayers());

			diffuseIrradianceRenderer.render(environmentCubemap);

			VulkanUtils::transitionImageLayout(
				context,
				diffuseIrradianceCubemap.getImage(),
				diffuseIrradianceCubemap.getImageFormat(),
				VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				0, diffuseIrradianceCubemap.getNumMipLevels(),
				0, diffuseIrradianceCubemap.getNumLayers());
		}

		std::array<const VulkanTexture*, 2> textures =
		{
			&environmentCubemap,
			&diffuseIrradianceCubemap,
		};

		int imageCount = static_cast<int>(swapChainContext.swapChainImageViews.size());


		for (size_t i = 0; i < imageCount; i++)
			for (int k = 0; k < textures.size(); k++)
				VulkanUtils::bindCombinedImageSampler(
					context,
					descriptorSets[i],
					k + 6,
					textures[k]->getImageView(),
					textures[k]->getSampler());
	}

	void Renderer::render(const UniformBufferObject* ubo, const RenderScene* scene, uint32_t imageIndex)
	{
		VkCommandBuffer commandBuffer = commandBuffers[imageIndex];
		VkFramebuffer frameBuffer = frameBuffers[imageIndex];
		VkDescriptorSet descriptorSet = descriptorSets[imageIndex];
		VkBuffer uniformBuffer = uniformBuffers[imageIndex];
		VkDeviceMemory uniformBufferMemory = uniformBuffersMemory[imageIndex];

		void* data = nullptr;
		vkMapMemory(context.device, uniformBufferMemory, 0, sizeof(ubo), 0, &data);
		memcpy(data, &ubo, sizeof(ubo));
		vkUnmapMemory(context.device, uniformBufferMemory);

		if (vkResetCommandBuffer(commandBuffer, 0) != VK_SUCCESS)
			throw std::runtime_error("Can't reset command buffer");

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		beginInfo.pInheritanceInfo = nullptr; // Optional

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
			throw std::runtime_error("Can't begin recording command buffer");

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderPass;
		renderPassInfo.framebuffer = frameBuffer;
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = swapChainContext.extent;

		std::array<VkClearValue, 3> clearValues = {};
		clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
		clearValues[1].color = { 0.0f, 0.0f, 0.0f, 1.0f };
		clearValues[2].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, skyBoxPipeline);
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
		{
			const VulkanMesh* skybox = scene->getSkybox();

			VkBuffer vertexBuffers[] = { skybox->getVertexBuffer() };
			VkBuffer indexBuffer = skybox->getIndexBuffer();
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
			vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);

			vkCmdDrawIndexed(commandBuffer, skybox->getNumIndices(), 1, 0, 0, 0);
		}
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pbrPipeline);
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
		{
			const VulkanMesh* mesh = scene->getMesh();

			VkBuffer vertexBuffers[] = { mesh->getVertexBuffer() };
			VkBuffer indexBuffer = mesh->getIndexBuffer();
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
			vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);

			vkCmdDrawIndexed(commandBuffer, mesh->getNumIndices(), 1, 0, 0, 0);
		}

		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);

		vkCmdEndRenderPass(commandBuffer);

		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
			throw std::runtime_error("Can't record command buffer");
	}

	void Renderer::update(UniformBufferObject* ubo, const RenderScene* scene)
	{

		static auto startTime = std::chrono::high_resolution_clock::now();
		auto currentTime = std::chrono::high_resolution_clock::now();

		const float rotationSpeed = 0.3f;
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

		const glm::vec3& up = { 0.0f, 0.0f, 1.0f };
		const glm::vec3& zero = { 0.0f, 0.0f, 0.0f };

		const float aspect = swapChainContext.extent.width / (float)swapChainContext.extent.height;
		const float zNear = 0.1f;
		const float zFar = 1000.0f;

		const glm::vec3& cameraPos = glm::vec3(2.0f, 2.0f, 2.0f);
		const glm::mat4& rotation = glm::rotate(glm::mat4(1.0f), time * rotationSpeed * glm::radians(90.0f), up);

		ubo->world = glm::rotate(glm::mat4(1.0f), time * rotationSpeed * glm::radians(90.0f), up); // mat4(1.0f);
		ubo->view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), zero, up);
		ubo->proj = glm::perspective(glm::radians(45.0f), aspect, zNear, zFar);
		ubo->proj[1][1] *= -1;
		ubo->cameraPosWS = glm::vec3(glm::vec4(cameraPos, 1.0f) * rotation); // glm::vec3(2.0f, 2.0f, 2.0f);

		// ImGui
		static float f = 0.0f;
		static int counter = 0;
		static bool show_demo_window = false;
		static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);

		ImGui::Begin("Material Parameters");

		int oldCurrentEnvironment = currentEnvironment;

		if (ImGui::BeginCombo("Choose your HDR map", scene->getHDRTexturePath(currentEnvironment)))
		{
			for (int i = 0; i < scene->getNumHDRTextures(); i++)
			{
				bool selected = (i == currentEnvironment);
				if (ImGui::Selectable(scene->getHDRTexturePath(i), &selected))
					currentEnvironment = i;
				if (selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		ImGui::Checkbox("Demo Window", &show_demo_window);

		ImGui::SliderFloat("Lerp User Material", &ubo->lerpUserValues, 0.0f, 1.0f);
		ImGui::SliderFloat("Metalness", &ubo->userMetalness, 0.0f, 1.0f);
		ImGui::SliderFloat("Roughness", &ubo->userRoughness, 0.0f, 1.0f);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();

		if (oldCurrentEnvironment != currentEnvironment)
			setEnvironment(scene, currentEnvironment);
	}

	void Renderer::shutdown()
	{
		for (auto uniformBuffer : uniformBuffers)
			vkDestroyBuffer(context.device, uniformBuffer, nullptr);

		uniformBuffers.clear();

		for (auto uniformBufferMemory : uniformBuffersMemory)
			vkFreeMemory(context.device, uniformBufferMemory, nullptr);

		uniformBuffersMemory.clear();

		for (auto framebuffer : frameBuffers)
			vkDestroyFramebuffer(context.device, framebuffer, nullptr);

		frameBuffers.clear();

		vkDestroyPipeline(context.device, pbrPipeline, nullptr);
		pbrPipeline = VK_NULL_HANDLE;

		vkDestroyPipeline(context.device, skyBoxPipeline, nullptr);
		skyBoxPipeline = VK_NULL_HANDLE;

		vkDestroyPipelineLayout(context.device, pipelineLayout, nullptr);
		pipelineLayout = VK_NULL_HANDLE;

		vkDestroyDescriptorSetLayout(context.device, descriptorSetLayout, nullptr);
		descriptorSetLayout = nullptr;

		vkDestroyRenderPass(context.device, renderPass, nullptr);
		renderPass = VK_NULL_HANDLE;

		vkFreeCommandBuffers(context.device, context.commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
		commandBuffers.clear();

		vkFreeDescriptorSets(context.device, context.descriptorPool, static_cast<uint32_t>(descriptorSets.size()), descriptorSets.data());
		descriptorSets.clear();

		hdriToCubeRenderer.shutdown();

		diffuseIrradianceRenderer.shutdown();

		environmentCubemap.clearGPUData();
		diffuseIrradianceCubemap.clearGPUData();

		ImGui_ImplVulkan_Shutdown();
	}
}