#version 450
#pragma shader_stage(fragment)

layout(location = 1) in vec2 texCoord;

layout(location = 0) out vec4 outColor;

void main() {
	outColor = vec4(1.0f);
}