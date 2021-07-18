#version 450
#pragma shader_stage(fragment)

#include "Common/brdf.inc"

layout(location = 0) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

vec2 IntegrateBRDF(float roughness, float dotNV)
{
	vec3 view;
	view.x = sqrt(1.0f - dotNV * dotNV);
	view.y = 0.0f;
	view.z = dotNV;

	vec3 normal = vec3(0.0f, 0.0f, 1.0f);

	float A = 0;
	float B = 0;

	const uint samples = 1024u;

	Surface sample_surface;
	sample_surface.view = view;
	sample_surface.normal = normal;
	sample_surface.dotNV = max(0.0f, dot(sample_surface.normal, sample_surface.view));

	for (uint i = 0; i < samples; ++i)
	{
		vec2 Xi = Hammersley(i, samples);

		vec3 halfVector = ImportanceSamplingGGX(Xi, normal, roughness);
		vec3 light = normalize(2.0 * dot(view, halfVector) * halfVector - view);

		float dotNL = max(light.z, 0.0);
		float dotHV = max(0.0f, dot(halfVector, view));
		float dotNH = max(0.0f, dot(normal, halfVector));

		if(dotNL > 0)
		{
			float G = GeometrySmith(normal, view, light, roughness);
			float G_Vis = (G * dotHV) / (dotNH * dotNV);
            float Fc = pow(1.0 - dotHV, 5.0);

			A += (1.0 - Fc) * G_Vis;
            B += Fc * G_Vis;
		}
	}

	return vec2(A,B) / float(samples);
}

void main()
{
	vec2 bakedColor = IntegrateBRDF(fragTexCoord.x, fragTexCoord.y);
	outColor = vec4(bakedColor.x, bakedColor.y , 0.0f, 0.0f);
}