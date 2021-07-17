#version 450
#pragma shader_stage(vertex)

// Input
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inTangent;
layout(location = 2) in vec3 inBinormal;
layout(location = 3) in vec3 inNormal;
layout(location = 4) in vec3 inColor;
layout(location = 5) in vec2 inTexCoord;

// Output
layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 fragPositionOS;

void main() {
	gl_Position = vec4(inPosition, 1.0f);

	fragColor = inColor;
	fragTexCoord = inTexCoord;
	fragPositionOS = inPosition;
}