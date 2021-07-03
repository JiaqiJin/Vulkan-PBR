#pragma once

#include <vulkan/vulkan.h>
#include <string>

#include "VulkanRendererContext.h"

#include <shaderc/shaderc.h>

namespace RHI
{
	enum class VulkanShaderKind
	{
		Vertex = 0,
		Fragment,
		Compute,
		Geometry,
		TessellationControl,
		TessellationEvaulation,
	};

	class VulkanShader
	{
	public:
		VulkanShader(const VulkanRendererContext& context)
			: context(context) { }

		~VulkanShader();

		bool compileFromFile(const char* path);
		bool compileFromFile(const char* path, VulkanShaderKind kind);
		void clear();

		inline VkShaderModule getShaderModule() const { return shaderModule; }

	private:
		bool compileFromSourceInternal(const char* path, const char* sourceData, size_t sourceSize, shaderc_shader_kind kind);

	private:
		VulkanRendererContext context;

		VkShaderModule shaderModule{ VK_NULL_HANDLE };
	};

}
