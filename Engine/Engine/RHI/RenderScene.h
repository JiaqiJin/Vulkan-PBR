#pragma once

#include <vulkan/vulkan.h>
#include <string>

#include "VulkanRendererContext.h"
#include "VulkanMesh.h"
#include "VulkanTexture.h"
#include "VulkanShader.h"

namespace RHI
{
	class RenderScene
	{
	public:
		RenderScene(const VulkanRendererContext& context)
			: context(context), mesh(context),
			albedoTexture(context), normalTexture(context), aoTexture(context),
			shadingTexture(context), emissionTexture(context), hdrTexture(context), 
			vertexShader(context), fragmentShader(context) { }

		void init(const std::string& vertexShaderFile,
			const std::string& fragmentShaderFile,
			const std::string& hdrFile,
			const std::string& albedoFile,
			const std::string& normalFile,
			const std::string& aoFile,
			const std::string& shadingFile,
			const std::string& emissionFile,
			const std::string& modelFile);

		void shutdown();

		inline const VulkanTexture& getAlbedoTexture() const { return albedoTexture; }
		inline const VulkanTexture& getNormalTexture() const { return normalTexture; }
		inline const VulkanTexture& getAOTexture() const { return aoTexture; }
		inline const VulkanTexture& getShadingTexture() const { return shadingTexture; }
		inline const VulkanTexture& getEmissionTexture() const { return emissionTexture; }
		inline const VulkanTexture& getHDRTexture() const { return hdrTexture; }
		inline const VulkanMesh& getMesh() const { return mesh; }
		inline const VulkanShader& getVertexShader() const { return vertexShader; }
		inline const VulkanShader& getFragmentShader() const { return fragmentShader; }

		VkShaderModule createShader(const std::string& path) const;

	private:
		VulkanRendererContext context;

		// Mesh
		VulkanMesh mesh;

		// Textures
		VulkanTexture albedoTexture;
		VulkanTexture normalTexture;
		VulkanTexture aoTexture;
		VulkanTexture shadingTexture;
		VulkanTexture emissionTexture;
		VulkanTexture hdrTexture;

		VulkanShader vertexShader;
		VulkanShader fragmentShader;
	};
}