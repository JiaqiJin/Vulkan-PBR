#version 450
#pragma shader_stage(fragment)

#include "Common/brdf.inc"

layout(location = 0) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main()
{
	outColor = vec4(1.0f);
}