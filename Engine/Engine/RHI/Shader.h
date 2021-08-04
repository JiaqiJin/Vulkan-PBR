#pragma once

#include <vulkan/vulkan.h>
#include "../Common/GraphicsEnums.h"
#include <string>
#include <memory>
#include <shaderc/shaderc.h>

namespace RHI
{
	class Device;

	class Shader
	{
	public:
		Shader(const std::shared_ptr<Device> device);

		~Shader();

		bool compileFromFile(const char* path);
		bool compileFromFile(const char* path, ShaderType type);
		bool reload();
		void clear();

		inline VkShaderModule getShaderModule() const { return shaderModule; }

	private:
		bool compileFromSourceInternal(const char* path, const char* sourceData, size_t sourceSize, shaderc_shader_kind kind);

	private:
		const std::shared_ptr<Device> device;

		ShaderType type;
		VkShaderModule shaderModule{ VK_NULL_HANDLE };

		std::string Shaderpath;
	};

}
