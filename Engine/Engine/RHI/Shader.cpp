#include "Shader.h"
#include "VulkanUtils.h"


#include <fstream>
#include <iostream>
#include <vector>

namespace RHI
{
	static shaderc_shader_kind vulkan_to_shaderc_kind(ShaderKind kind)
	{
		switch (kind)
		{
		case ShaderKind::Vertex: return shaderc_vertex_shader;
		case ShaderKind::Fragment: return shaderc_fragment_shader;
		case ShaderKind::Compute: return shaderc_compute_shader;
		case ShaderKind::Geometry: return shaderc_geometry_shader;
		case ShaderKind::TessellationControl: return shaderc_tess_control_shader;
		case ShaderKind::TessellationEvaulation: return shaderc_tess_evaluation_shader;
		}

		throw std::runtime_error("Unsupported shader kind");
	}

	Shader::~Shader()
	{
		clear();
	}

	bool Shader::compileFromFile(const char* path)
	{
		std::ifstream file(path, std::ios::ate | std::ios::binary);

		if (!file.is_open())
		{
			std::cerr << "Shader::loadFromFile(): can't load shader at \"" << path << "\"" << std::endl;
			return false;
		}

		size_t fileSize = static_cast<size_t>(file.tellg());
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);
		file.close();

		return compileFromSourceInternal(path, buffer.data(), buffer.size(), shaderc_glsl_infer_from_source);
	}

	bool Shader::compileFromFile(const char* path, ShaderKind kind)
	{
		std::ifstream file(path, std::ios::ate | std::ios::binary);

		if (!file.is_open())
		{
			std::cerr << "Shader::loadFromFile(): can't load shader at \"" << path << "\"" << std::endl;
			return false;
		}

		size_t fileSize = static_cast<size_t>(file.tellg());
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);
		file.close();

		return compileFromSourceInternal(path, buffer.data(), buffer.size(), vulkan_to_shaderc_kind(kind));
	}

	bool Shader::compileFromSourceInternal(const char* path, const char* sourceData, size_t sourceSize, shaderc_shader_kind kind)
	{
		// convert glsl/hlsl code to SPIR-V bytecode
		shaderc_compiler_t compiler = shaderc_compiler_initialize();
		shaderc_compilation_result_t result = shaderc_compile_into_spv(
			compiler,
			sourceData, sourceSize,
			shaderc_glsl_infer_from_source,
			path,
			"main",
			nullptr
		);

		if (shaderc_result_get_compilation_status(result) != shaderc_compilation_status_success)
		{
			std::cerr << "Shader::loadFromFile(): can't compile shader at \"" << path << "\"" << std::endl;
			std::cerr << "\t" << shaderc_result_get_error_message(result);

			shaderc_result_release(result);
			shaderc_compiler_release(compiler);

			return false;
		}

		size_t size = shaderc_result_get_length(result);
		const uint32_t* data = reinterpret_cast<const uint32_t*>(shaderc_result_get_bytes(result));

		clear();
		shaderModule = VulkanUtils::createShaderModule(context, data, size);

		shaderc_result_release(result);
		shaderc_compiler_release(compiler);

		return true;
	}


	void Shader::clear()
	{
		vkDestroyShaderModule(context.device, shaderModule, nullptr);
		shaderModule = VK_NULL_HANDLE;
	}
}