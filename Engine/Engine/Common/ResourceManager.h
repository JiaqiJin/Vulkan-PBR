#pragma once

#include <unordered_map>

namespace RHI
{
	enum class ShaderKind;
	class Shader;
	class VulkanContext;
}

class Mesh;
class Texture;

class ResourceManager
{
public:
	ResourceManager(const RHI::VulkanContext* context);

	Mesh* getMesh(unsigned int id) const;
	Mesh* createCubeMesh(unsigned int id, float size);
	Mesh* loadMesh(unsigned int id, const char* path);
	void unloadMesh(unsigned int id);

	RHI::Shader* getShader(unsigned int id) const;
	RHI::Shader* loadShader(unsigned int id, const char* path);
	RHI::Shader* loadShader(unsigned int id, RHI::ShaderKind kind, const char* path);
	bool reloadShader(unsigned int id);
	void unloadShader(unsigned int id);

	Texture* getTexture(unsigned int id) const;
	Texture* loadTexture(unsigned int id, const char* path);
	void unloadTexture(unsigned int id);

private:
	const RHI::VulkanContext* context { nullptr };
	
	std::unordered_map<unsigned int, Mesh*> meshes;
	std::unordered_map<unsigned int, RHI::Shader*> shaders;
	std::unordered_map<unsigned int, Texture*> textures;
};

