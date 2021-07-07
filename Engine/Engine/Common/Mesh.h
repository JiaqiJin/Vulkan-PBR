#pragma once

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

#include <array>
#include <vector>
#include <string>

#include "../RHI/RendererContext.h"

class Mesh
{
public:
	Mesh(const RendererContext& context)
		: context(context) { }

	~Mesh();

	// How passing model data format to vertex shader
	static VkVertexInputBindingDescription getVertexInputBindingDescription();
	static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

	inline VkBuffer getVertexBuffer() const { return vertexBuffer; }
	inline VkBuffer getIndexBuffer() const { return indexBuffer; }
	inline uint32_t getNumIndices() const { return static_cast<uint32_t>(indices.size()); }

	bool loadFromFile(const std::string& filename);

	void createSkybox(float size);
	void createQuad(float size);

	void uploadToGPU();
	void clearGPUData();
	void clearCPUData();

private:
	void createVertexBuffer();
	void createIndexBuffer();

private:
	RendererContext context;

	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 tangent;
		glm::vec3 binormal;
		glm::vec3 normal;
		glm::vec3 color;
		glm::vec2 uv;
	};

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	VkBuffer vertexBuffer{ VK_NULL_HANDLE };
	VkDeviceMemory vertexBufferMemory{ VK_NULL_HANDLE };

	VkBuffer indexBuffer{ VK_NULL_HANDLE };
	VkDeviceMemory indexBufferMemory{ VK_NULL_HANDLE };
};