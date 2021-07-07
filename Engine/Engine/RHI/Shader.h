#pragma once

#include <vulkan/vulkan.h>
#include <string>

#include "RendererContext.h"

#include <shaderc/shaderc.h>

namespace RHI
{
	enum class ShaderKind
	{
		Vertex = 0,
		Fragment,
		Compute,
		Geometry,
		TessellationControl,
		TessellationEvaulation,
	};

	class Shader
	{
	public:
		Shader(const RendererContext& context)
			: context(context) { }

		~Shader();

		bool compileFromFile(const char* path);
		bool compileFromFile(const char* path, ShaderKind kind);
		void clear();

		inline VkShaderModule getShaderModule() const { return shaderModule; }

	private:
		bool compileFromSourceInternal(const char* path, const char* sourceData, size_t sourceSize, shaderc_shader_kind kind);

	private:
		RendererContext context;

		VkShaderModule shaderModule{ VK_NULL_HANDLE };
	};

}
