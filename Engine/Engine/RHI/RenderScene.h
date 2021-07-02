#pragma once

#include <vulkan/vulkan.h>
#include <string>

#include "VulkanRendererContext.h"
#include "../Common/ResourceManager.h"

namespace RHI
{
	namespace config
	{
		enum Meshes
		{
			Helmet = 0,
			Skybox,
		};

		enum Shaders
		{
			PBRVertex = 0,
			PBRFragment,
			SkyboxVertex,
			SkyboxFragment,
			CubeVertex,
			HDRIToCubeFragment,
			DiffuseIrradianceFragment,
		};

		enum Textures
		{
			Albedo = 0,
			Normal,
			AO,
			Shading,
			Emission,
			Environment,
		};
	}

	class RenderScene
	{
	public:
		RenderScene(const VulkanRendererContext& context)
			: resources(context) { }

		void init();

		void shutdown();


		inline const VulkanShader* getPBRVertexShader() const { return resources.getShader(config::Shaders::PBRVertex); }
		inline const VulkanShader* getPBRFragmentShader() const { return resources.getShader(config::Shaders::PBRFragment); }
		inline const VulkanShader* getSkyboxVertexShader() const { return resources.getShader(config::Shaders::SkyboxVertex); }
		inline const VulkanShader* getSkyboxFragmentShader() const { return resources.getShader(config::Shaders::SkyboxFragment); }
		inline const VulkanShader* getCubeVertexShader() const { return resources.getShader(config::Shaders::CubeVertex); }
		inline const VulkanShader* getHDRIToFragmentShader() const { return resources.getShader(config::Shaders::HDRIToCubeFragment); }
		inline const VulkanShader* getDiffuseIrradianceFragmentShader() const { return resources.getShader(config::Shaders::DiffuseIrradianceFragment); }

		inline const VulkanTexture* getAlbedoTexture() const { return resources.getTexture(config::Textures::Albedo); }
		inline const VulkanTexture* getNormalTexture() const { return resources.getTexture(config::Textures::Normal); }
		inline const VulkanTexture* getAOTexture() const { return resources.getTexture(config::Textures::AO); }
		inline const VulkanTexture* getShadingTexture() const { return resources.getTexture(config::Textures::Shading); }
		inline const VulkanTexture* getEmissionTexture() const { return resources.getTexture(config::Textures::Emission); }
		inline const VulkanTexture* getHDRTexture() const { return resources.getTexture(config::Textures::Environment); }

		inline const VulkanMesh* getMesh() const { return resources.getMesh(config::Meshes::Helmet); }
		inline const VulkanMesh* getSkybox() const { return resources.getMesh(config::Meshes::Skybox); }

		VkShaderModule createShader(const std::string& path) const;

	private:
		ResourceManager resources;
	};
}