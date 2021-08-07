#pragma once

#include <cstdint>

enum class BufferType : uint8_t
{
	STATIC = 0,
	DYNAMIC,

	MAX,
};

enum class RenderPrimitiveType : uint8_t
{
	POINT_LIST = 0,
	LINE_LIST,
	LINE_PATCH,
	LINE_STRIP,
	TRIANGLE_LIST,
	TRIANGLE_PATCH,
	TRIANGLE_STRIP,
	TRIANGLE_FAN,
	QUAD_PATCH,

	MAX,
};

enum class IndexFormat : uint8_t
{
	UINT16 = 0,
	UINT32,

	MAX,
};

enum class Format : uint16_t
{
	UNDEFINED = 0,

	// 8-bit formats
	R8_UNORM,
	R8_SNORM,
	R8_UINT,
	R8_SINT,

	R8G8_UNORM,
	R8G8_SNORM,
	R8G8_UINT,
	R8G8_SINT,

	R8G8B8_UNORM,
	R8G8B8_SNORM,
	R8G8B8_UINT,
	R8G8B8_SINT,

	B8G8R8_UNORM,
	B8G8R8_SNORM,
	B8G8R8_UINT,
	B8G8R8_SINT,

	R8G8B8A8_UNORM,
	R8G8B8A8_SNORM,
	R8G8B8A8_UINT,
	R8G8B8A8_SINT,

	B8G8R8A8_UNORM,
	B8G8R8A8_SNORM,
	B8G8R8A8_UINT,
	B8G8R8A8_SINT,

	// 16-bit formats
	R16_UNORM,
	R16_SNORM,
	R16_UINT,
	R16_SINT,
	R16_SFLOAT,

	R16G16_UNORM,
	R16G16_SNORM,
	R16G16_UINT,
	R16G16_SINT,
	R16G16_SFLOAT,

	R16G16B16_UNORM,
	R16G16B16_SNORM,
	R16G16B16_UINT,
	R16G16B16_SINT,
	R16G16B16_SFLOAT,

	R16G16B16A16_UNORM,
	R16G16B16A16_SNORM,
	R16G16B16A16_UINT,
	R16G16B16A16_SINT,
	R16G16B16A16_SFLOAT,

	// 32-bit formats
	R32_UINT,
	R32_SINT,
	R32_SFLOAT,

	R32G32_UINT,
	R32G32_SINT,
	R32G32_SFLOAT,

	R32G32B32_UINT,
	R32G32B32_SINT,
	R32G32B32_SFLOAT,

	R32G32B32A32_UINT,
	R32G32B32A32_SINT,
	R32G32B32A32_SFLOAT,

	// depth formats
	D16_UNORM,
	D16_UNORM_S8_UINT,
	D24_UNORM_S8_UINT,
	D32_SFLOAT,
	D32_SFLOAT_S8_UINT,

	MAX,
};


enum class ShaderType : uint8_t
{
	// Graphics pipeline
	VERTEX = 0,
	TESSELLATION_CONTROL,
	TESSELLATION_EVALUATION,
	GEOMETRY,
	FRAGMENT,

	// Compute pipeline
	COMPUTE,

	// Raytracing pipeline
	RAY_GENERATION,
	INTERSECTION,
	ANY_HIT,
	CLOSEST_HIT,
	MISS,
	CALLABLE,

	MAX,
};

enum FrameBufferAttachmentType : uint8_t
{
	COLOR = 0,
	DEPTH,
	SWAP_CHAIN_COLOR,
};

struct FrameBufferAttachment
{
	FrameBufferAttachmentType type{ FrameBufferAttachmentType::COLOR };
};