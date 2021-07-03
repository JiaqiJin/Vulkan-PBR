#pragma once

#include <unordered_map>

#include "../RHI/VulkanRendererContext.h"

namespace RHI
{
	enum class VulkanShaderKind;

	class VulkanMesh;
	class VulkanShader;
	class VulkanTexture;
}

class ResourceManager
{
public:
	ResourceManager(const VulkanRendererContext& context);

	RHI::VulkanMesh* getMesh(int id) const;
	RHI::VulkanMesh* createCubeMesh(int id, float size);
	RHI::VulkanMesh* loadMesh(int id, const char* path);
	void unloadMesh(int id);

	RHI::VulkanShader* getShader(int id) const;
	RHI::VulkanShader* loadShader(int id, RHI::VulkanShaderKind kind, const char* path);
	void unloadShader(int id);

	RHI::VulkanTexture* getTexture(int id) const;
	RHI::VulkanTexture* loadTexture(int id, const char* path);
	void unloadTexture(int id);

private:
	VulkanRendererContext context;
	
	std::unordered_map<int, RHI::VulkanMesh*> meshes;
	std::unordered_map<int, RHI::VulkanShader*> shaders;
	std::unordered_map<int, RHI::VulkanTexture*> textures;
};

