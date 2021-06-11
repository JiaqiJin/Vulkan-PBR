#pragma once

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

#include <array>
#include <vector>
#include <string>

#include "VulkanRendererContext.h"

namespace RHI
{
	class VulkanMesh
	{
	public:
		VulkanMesh(const VulkanRendererContext& context)
			: context(context) { }

		~VulkanMesh();

		// How passing model data format to vertex shader
		static VkVertexInputBindingDescription getVertexInputBindingDescription();
		static std::array<VkVertexInputAttributeDescription, 6> getAttributeDescriptions();

		bool loadFromFile(const std::string& filename);

		void uploadToGPU();
		void clearGPUData();
		void clearCPUData();

	private:
		void createVertexBuffer();
		void createIndexBuffer();

	private:
		VulkanRendererContext context;

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
}