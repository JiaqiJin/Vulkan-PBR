#include "Mesh.h"
#include "../RHI/VulkanUtils.h"
#include "../RHI/VulkanContext.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>

using namespace RHI;

Mesh::~Mesh()
{
	clearGPUData();
	clearCPUData();
}

VkVertexInputBindingDescription Mesh::getVertexInputBindingDescription()
{
	VkVertexInputBindingDescription bindingDescription;
	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof(Vertex);
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	return bindingDescription;
}

std::vector<VkVertexInputAttributeDescription> Mesh::getAttributeDescriptions()
{
	static std::vector<VkVertexInputAttributeDescription> attributes = {
	{ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position) },
	{ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, tangent) },
	{ 2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, binormal) },
	{ 3, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal) },
	{ 4, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color) },
	{ 5, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv) }
	};

	return attributes;
}

bool Mesh::loadFromFile(const std::string& path)
{
	Assimp::Importer importer;

	unsigned int flags = aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType;
	const aiScene* scene = importer.ReadFile(path, flags);

	if (!scene)
	{
		std::cerr << "Mesh::loadFromFile(): " << importer.GetErrorString() << std::endl;
		return false;
	}

	if (!scene->HasMeshes())
	{
		std::cerr << "Mesh::loadFromFile(): model has no meshes" << std::endl;
		return false;
	}

	aiMesh* mesh = scene->mMeshes[0];
	assert(mesh != nullptr);

	// Fill CPU data
	vertices.resize(mesh->mNumVertices);
	indices.resize(mesh->mNumFaces * 3);

	// Vertices
	aiVector3D* meshVertices = mesh->mVertices;
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		vertices[i].position = glm::vec3(meshVertices[i].x, meshVertices[i].y, meshVertices[i].z);

	// Tangents
	aiVector3D* meshTangents = mesh->mTangents;
	if (meshTangents)
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
			vertices[i].tangent = glm::vec3(meshTangents[i].x, meshTangents[i].y, meshTangents[i].z);
	else
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
			vertices[i].tangent = glm::vec3(0.0f, 0.0f, 0.0f);

	// Bitangents
	aiVector3D* meshBinormals = mesh->mBitangents;
	if (meshBinormals)
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
			vertices[i].binormal = glm::vec3(meshBinormals[i].x, meshBinormals[i].y, meshBinormals[i].z);
	else
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
			vertices[i].binormal = glm::vec3(0.0f, 0.0f, 0.0f);

	// Normal
	aiVector3D* meshNormals = mesh->mNormals;
	if (meshNormals)
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
			vertices[i].normal = glm::vec3(meshNormals[i].x, meshNormals[i].y, meshNormals[i].z);
	else
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
			vertices[i].normal = glm::vec3(0.0f, 0.0f, 0.0f);

	// UV
	aiVector3D* meshUVs = mesh->mTextureCoords[0];
	if (meshUVs)
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
			vertices[i].uv = glm::vec2(meshUVs[i].x, 1.0f - meshUVs[i].y);
	else
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
			vertices[i].uv = glm::vec2(0.0f, 0.0f);

	// Color
	aiColor4D* meshColors = mesh->mColors[0];
	if (meshColors)
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
			vertices[i].color = glm::vec3(meshColors[i].r, meshColors[i].g, meshColors[i].b);
	else
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
			vertices[i].color = glm::vec3(1.0f, 1.0f, 1.0f);

	// Indices
	aiFace* meshFaces = mesh->mFaces;
	unsigned int index = 0;
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		for (unsigned int faceIndex = 0; faceIndex < meshFaces[i].mNumIndices; faceIndex++)
			indices[index++] = meshFaces[i].mIndices[faceIndex];

	// Upload CPU data to GPU
	clearGPUData();
	uploadToGPU();

	// TODO: should we clear CPU data after uploading it to the GPU?

	return true;
}

void Mesh::createSkybox(float size)
{
	clearCPUData();
	clearGPUData();

	vertices.resize(8);
	indices.resize(36);

	float halfSize = size * 0.5f;

	vertices[0].position = glm::vec3(-halfSize, -halfSize, -halfSize);
	vertices[1].position = glm::vec3(halfSize, -halfSize, -halfSize);
	vertices[2].position = glm::vec3(halfSize, halfSize, -halfSize);
	vertices[3].position = glm::vec3(-halfSize, halfSize, -halfSize);
	vertices[4].position = glm::vec3(-halfSize, -halfSize, halfSize);
	vertices[5].position = glm::vec3(halfSize, -halfSize, halfSize);
	vertices[6].position = glm::vec3(halfSize, halfSize, halfSize);
	vertices[7].position = glm::vec3(-halfSize, halfSize, halfSize);

	indices = {
	0, 1, 2, 2, 3, 0,
	1, 5, 6, 6, 2, 1,
	3, 2, 6, 6, 7, 3,
	5, 4, 6, 4, 7, 6,
	1, 0, 4, 4, 5, 1,
	4, 0, 3, 3, 7, 4,
	};

	uploadToGPU();
}

void Mesh::createQuad(float size)
{
	clearCPUData();
	clearGPUData();

	vertices.resize(4);
	indices.resize(6);

	float halfSize = size * 0.5f;

	vertices[0].position = glm::vec3(-halfSize, -halfSize, 0.0f);
	vertices[1].position = glm::vec3(halfSize, -halfSize, 0.0f);
	vertices[2].position = glm::vec3(halfSize, halfSize, 0.0f);
	vertices[3].position = glm::vec3(-halfSize, halfSize, 0.0f);

	vertices[0].uv = glm::vec2(0.0f, 0.0f);
	vertices[1].uv = glm::vec2(1.0f, 0.0f);
	vertices[2].uv = glm::vec2(1.0f, 1.0f);
	vertices[3].uv = glm::vec2(0.0f, 1.0f);

	indices = {
		1, 0, 2, 3, 2, 0,
	};

	uploadToGPU();
}

void Mesh::createVertexBuffer()
{
	VkDeviceSize bufferSize = sizeof(Vertex) * vertices.size();

	// StagingBuffer definition
	VkBuffer stagingBuffer = VK_NULL_HANDLE;
	VkDeviceMemory stagingBufferMemory = VK_NULL_HANDLE;

	VulkanUtils::createBuffer(
		context,
		bufferSize,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,  // Buffer can be used as the destination of transfer command
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, // Specifies that memory allocated with this type is the most efficient for device access
		vertexBuffer,
		vertexBufferMemory);

	// Create staging buffer
	VulkanUtils::createBuffer(
		context,
		bufferSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT, // The buffer can be used as the source of transfer command
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, // The memory allocated can be mapped for host access using vkMapMemory.
		stagingBuffer,
		stagingBufferMemory);

	// Fill staging buffer
	void* data = nullptr;
	vkMapMemory(context->getDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
	vkUnmapMemory(context->getDevice(), stagingBufferMemory);

	// Transfer to GPU local memory
	VulkanUtils::copyBuffer(context, stagingBuffer, vertexBuffer, bufferSize);

	// Destroy staging buffer
	vkDestroyBuffer(context->getDevice(), stagingBuffer, nullptr);
	vkFreeMemory(context->getDevice(), stagingBufferMemory, nullptr);
}

void Mesh::createIndexBuffer()
{
	VkDeviceSize bufferSize = sizeof(uint32_t) * indices.size();

	VkBuffer stagingBuffer = VK_NULL_HANDLE;
	VkDeviceMemory stagingBufferMemory = VK_NULL_HANDLE;

	VulkanUtils::createBuffer(
		context,
		bufferSize,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, // Buffer can be used as the destination of transfer command
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, // Specifies that memory allocated with this type is the most efficient for device access
		indexBuffer,
		indexBufferMemory);

	// Create staging buffer
	VulkanUtils::createBuffer(
		context,
		bufferSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT, // The buffer can be used as the source of transfer command
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, // The memory allocated can be mapped for host access using vkMapMemory.
		stagingBuffer,
		stagingBufferMemory);

	// Fill staging buffer
	void* data = nullptr;
	vkMapMemory(context->getDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
	vkUnmapMemory(context->getDevice(), stagingBufferMemory);

	// Transfer to GPU local memory
	VulkanUtils::copyBuffer(context, stagingBuffer, indexBuffer, bufferSize);

	// Destroy staging buffer
	vkDestroyBuffer(context->getDevice(), stagingBuffer, nullptr);
	vkFreeMemory(context->getDevice(), stagingBufferMemory, nullptr);
}

void Mesh::uploadToGPU()
{
	createVertexBuffer();
	createIndexBuffer();
}

void Mesh::clearGPUData()
{
	vkDestroyBuffer(context->getDevice(), vertexBuffer, nullptr);
	vertexBuffer = VK_NULL_HANDLE;

	vkFreeMemory(context->getDevice(), vertexBufferMemory, nullptr);
	vertexBufferMemory = VK_NULL_HANDLE;

	vkDestroyBuffer(context->getDevice(), indexBuffer, nullptr);
	indexBuffer = VK_NULL_HANDLE;

	vkFreeMemory(context->getDevice(), indexBufferMemory, nullptr);
	indexBufferMemory = VK_NULL_HANDLE;
}

void Mesh::clearCPUData()
{
	vertices.clear();
	indices.clear();
}