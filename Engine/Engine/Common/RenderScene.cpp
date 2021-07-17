#include "RenderScene.h"
#include "../RHI/Shader.h"

#include <fstream>
#include <iostream>
#include <vector>

#include <cassert>

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
			 "Assert/Shader/diffuseIrradiance.frag",
			 "Assert/Shader/bakeBRDF.vert",
			 "Assert/Shader/bakeBRDF.frag"
		};

		static std::vector<const char*> textures = {
			"Assert/Texture/Default_albedo.jpg",
			"Assert/Texture/Default_normal.jpg",
			"Assert/Texture/Default_AO.jpg",
			"Assert/Texture/Default_metalRoughness.jpg",
			"Assert/Texture/Default_emissive.jpg",
		};

		static std::vector<const char*> hdrTextures = {
			"Assert/Texture/Ice_Lake/Ice_Lake_Ref.hdr",
			"Assert/Texture/Default_environment.hdr"
		};
	}

	void RenderScene::init()
	{
		for (int i = 0; i < config::meshes.size(); i++)
			resources.loadMesh(i, config::meshes[i]);

		resources.createCubeMesh(config::Meshes::Skybox, 1000.0f);

		for (int i = 0; i < config::shaders.size(); i++)
			resources.loadShader(i, config::shaders[i]);

		for (int i = 0; i < config::textures.size(); i++)
			resources.loadTexture(i, config::textures[i]);

		for (int i = 0; i < config::hdrTextures.size(); i++)
			resources.loadTexture(config::Textures::Environment + i, config::hdrTextures[i]);
	}

	const char* RenderScene::getHDRTexturePath(int index) const
	{
		assert(index >= 0 && index < config::hdrTextures.size());
		return config::hdrTextures[index];
	}

	size_t RenderScene::getNumHDRTextures() const
	{
		return config::hdrTextures.size();
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

		for (int i = 0; i < config::hdrTextures.size(); i++)
			resources.unloadTexture(config::Textures::Environment + i);
	}

	void RenderScene::reloadShaders()
	{
		for (int i = 0; i < config::shaders.size(); i++)
			resources.reloadShader(i);
	}
}

