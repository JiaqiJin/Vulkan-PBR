//#pragma once
//
//#include <vulkan/vulkan.h>
//#include <string>
//
//#include "../RHI/VulkanContext.h"
//#include "ResourceManager.h"
//
//namespace RHI
//{
//	namespace config
//	{
//		enum Meshes
//		{
//			Helmet = 0,
//			Skybox,
//		};
//
//		enum Shaders
//		{
//			PBRVertex = 0,
//			PBRFragment,
//			SkyboxVertex,
//			SkyboxFragment,
//			CubeVertex,
//			HDRIToCubeFragment,
//			DiffuseIrradianceFragment,
//			BakedBRDFVertex,
//			BakedBRDFFragment,
//		};
//
//		enum Textures
//		{
//			Albedo = 0,
//			Normal,
//			AO,
//			Shading,
//			Emission,
//			Environment,
//		};
//	}
//
//	class RenderScene
//	{
//	public:
//		RenderScene(const VulkanContext* context)
//			: resources(context) { }
//
//		void init();
//
//		void shutdown();
//
//		inline const Shader* getPBRVertexShader() const { return resources.getShader(config::Shaders::PBRVertex); }
//		inline const Shader* getPBRFragmentShader() const { return resources.getShader(config::Shaders::PBRFragment); }
//
//		inline const Shader* getSkyboxVertexShader() const { return resources.getShader(config::Shaders::SkyboxVertex); }
//		inline const Shader* getSkyboxFragmentShader() const { return resources.getShader(config::Shaders::SkyboxFragment); }
//
//		inline const Shader* getCubeVertexShader() const { return resources.getShader(config::Shaders::CubeVertex); }
//		inline const Shader* getHDRIToFragmentShader() const { return resources.getShader(config::Shaders::HDRIToCubeFragment); }
//		inline const Shader* getDiffuseIrradianceFragmentShader() const { return resources.getShader(config::Shaders::DiffuseIrradianceFragment); }
//
//		inline const Shader* getBakedBRDFVertexShader() const { return resources.getShader(config::Shaders::BakedBRDFVertex); }
//		inline const Shader* getBakedBRDFFragmentShader() const { return resources.getShader(config::Shaders::BakedBRDFFragment); }
//
//		inline const Texture* getAlbedoTexture() const { return resources.getTexture(config::Textures::Albedo); }
//		inline const Texture* getNormalTexture() const { return resources.getTexture(config::Textures::Normal); }
//		inline const Texture* getAOTexture() const { return resources.getTexture(config::Textures::AO); }
//		inline const Texture* getShadingTexture() const { return resources.getTexture(config::Textures::Shading); }
//		inline const Texture* getEmissionTexture() const { return resources.getTexture(config::Textures::Emission); }
//		inline const Texture* getHDRTexture(int index) const { return resources.getTexture(config::Textures::Environment + index); }
//
//		inline const Mesh* getMesh() const { return resources.getMesh(config::Meshes::Helmet); }
//		inline const Mesh* getSkybox() const { return resources.getMesh(config::Meshes::Skybox); }
//
//		const char* getHDRTexturePath(int index) const;
//		size_t getNumHDRTextures() const;
//
//		void reloadShaders();
//
//	private:
//		ResourceManager resources;
//	};
//}