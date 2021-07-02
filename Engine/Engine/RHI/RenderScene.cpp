#include "RenderScene.h"
#include "VulkanShader.h"

#include <fstream>
#include <iostream>
#include <vector>

namespace RHI
{
	namespace config
	{
		static std::vector<const char*> meshes = {
		"Assert/Model/DamagedHelmet.fbx"
		};

		static std::vector<const char*> shaders = {
			 "Assert/Shader/Pbrshader.vert",
			 "Assert/Shader/Pbrshader.frag",
			 "Assert/Shader/skyBox.vert",
			 "Assert/Shader/skyBox.frag",
			 "Assert/Shader/commonCube.vert",
			 "Assert/Shader/hdriToCube.frag",
			 "Assert/Shader/diffuseIrradiance.frag"
		};

		static std::vector<VulkanShaderKind> shaderKinds = {
			VulkanShaderKind::Vertex,
			VulkanShaderKind::Fragment,
			VulkanShaderKind::Vertex,
			VulkanShaderKind::Fragment,
			VulkanShaderKind::Vertex,
			VulkanShaderKind::Fragment,
			VulkanShaderKind::Fragment,
		};

		static std::vector<const char*> textures = {
			"Assert/Texture/Default_albedo.jpg",
			"Assert/Texture/Default_normal.jpg",
			"Assert/Texture/Default_AO.jpg",
			"Assert/Texture/Default_metalRoughness.jpg",
			"Assert/Texture/Default_emissive.jpg",
		};

		static const char* hdrTexture = "Assert/Texture/Ice_Lake/Ice_Lake_Ref.hdr";
	}

	void RenderScene::init()
	{
		for (int i = 0; i < config::meshes.size(); i++)
			resources.loadMesh(i, config::meshes[i]);

		resources.createCubeMesh(config::Meshes::Skybox, 1000.0f);

		for (int i = 0; i < config::shaders.size(); i++)
			resources.loadShader(i, config::shaderKinds[i], config::shaders[i]);

		for (int i = 0; i < config::textures.size(); i++)
			resources.loadTexture(i, config::textures[i]);

		resources.loadHDRTexture(config::Textures::Environment, config::hdrTexture);
	}

	void RenderScene::shutdown()
	{
		for (int i = 0; i < config::meshes.size(); i++)
			resources.unloadMesh(i);

		resources.unloadMesh(config::Meshes::Skybox);

		for (int i = 0; i < config::shaders.size(); i++)
			resources.unloadShader(i);

		for (int i = 0; i < config::textures.size(); i++)
			resources.unloadTexture(i);

		resources.unloadTexture(config::Textures::Environment);
	}
}