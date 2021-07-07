#pragma once

#include <unordered_map>

#include "../RHI/RendererContext.h"

namespace RHI
{
	enum class ShaderKind;
	class Shader;
}

class Mesh;
class Texture;

class ResourceManager
{
public:
	ResourceManager(const RendererContext& context);

	Mesh* getMesh(int id) const;
	Mesh* createCubeMesh(int id, float size);
	Mesh* loadMesh(int id, const char* path);
	void unloadMesh(int id);

	RHI::Shader* getShader(int id) const;
	RHI::Shader* loadShader(int id, const char* path);
	RHI::Shader* loadShader(int id, RHI::ShaderKind kind, const char* path);
	void unloadShader(int id);

	Texture* getTexture(int id) const;
	Texture* loadTexture(int id, const char* path);
	void unloadTexture(int id);

private:
	RendererContext context;
	
	std::unordered_map<int, Mesh*> meshes;
	std::unordered_map<int, RHI::Shader*> shaders;
	std::unordered_map<int, Texture*> textures;
};

