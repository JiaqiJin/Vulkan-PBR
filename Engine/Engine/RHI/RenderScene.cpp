#include "RenderScene.h"

#include <fstream>
#include <iostream>
#include <vector>

namespace RHI
{
	static std::vector<char> readFile(const std::string& filename)
	{
		std::ifstream file(filename, std::ios::ate | std::ios::binary);

		if (!file.is_open())
			throw std::runtime_error("Can't open file");

		size_t fileSize = static_cast<size_t>(file.tellg());
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);
		file.close();

		return buffer;
	}

	void RenderScene::init(
		const std::string& vertexShaderFile,
		const std::string& fragmentShaderFile,
		const std::string& hdrFile,
		const std::string& albedoFile,
		const std::string& normalFile,
		const std::string& aoFile,
		const std::string& shadingFile,
		const std::string& emissionFile,
		const std::string& modelFile)
	{
		vertexShader.compileFromFile(vertexShaderFile, VulkanShaderKind::Vertex);
		fragmentShader.compileFromFile(fragmentShaderFile, VulkanShaderKind::Fragment);
		mesh.loadFromFile(modelFile);
		albedoTexture.loadFromFile(albedoFile);
		normalTexture.loadFromFile(normalFile);
		aoTexture.loadFromFile(aoFile);
		shadingTexture.loadFromFile(shadingFile);
		emissionTexture.loadFromFile(emissionFile);
		hdrTexture.loadHDRFromFile(hdrFile);
	}

	void RenderScene::shutdown()
	{
		albedoTexture.clearGPUData();
		normalTexture.clearGPUData();
		aoTexture.clearGPUData();
		hdrTexture.clearGPUData();
		shadingTexture.clearGPUData();
		emissionTexture.clearGPUData();
		mesh.clearGPUData();
		vertexShader.clear();
		fragmentShader.clear();
	}

	VkShaderModule RenderScene::createShader(const std::string& path) const
	{
		std::vector<char> code = readFile(path);

		VkShaderModuleCreateInfo shaderInfo = {};
		shaderInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		shaderInfo.codeSize = code.size();
		shaderInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		VkShaderModule shader;
		if (vkCreateShaderModule(context.device, &shaderInfo, nullptr, &shader) != VK_SUCCESS)
			throw std::runtime_error("Can't create shader module");

		return shader;
	}
}