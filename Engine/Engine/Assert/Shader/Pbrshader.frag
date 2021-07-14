#version 450
#pragma shader_stage(fragment)

#include "Common/Uniform.inc"
#include "Common/SceneTextures.inc"
#include "Common/brdf.inc"

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragTangentWS;
layout(location = 3) in vec3 fragBinormalWS;
layout(location = 4) in vec3 fragNormalWS;
layout(location = 5) in vec3 fragPositionWS;

layout(location = 0) out vec4 outColor;

// ---------- PBR ---------------------

struct MicrofacetMaterial
{
	vec3 albedo;
	float roughness;
	float metalness;
	vec3 f0;
};

// Cook-Torrance reflectance equation
// f_cookTorrance = DFG / 4(wo · n)(wi · n)

vec3 MicrofacetBRDF(Surface surface, MicrofacetMaterial material)
{
	// simplifying assumption that most dielectric surfaces look visually correct with a constant F0 0.04.
	// while do specify f0 for metallic surfaces as then given by the albedo value.
	//vec3 f0 = lerp(vec3(0.04f), material.albedo, material.metalness);

	float D = DistributionGGX(surface, material.roughness);
	vec3 F = F_Shlick(surface, material.f0);
	float G= G_SmithGGX(surface, material.roughness);

	vec3 specular_reflection = D * F * G;
	vec3 diffuse_reflection = material.albedo * lerp(vec3(1.0f) - F, vec3(0.0f), material.metalness);
	
	return (diffuse_reflection * iPI + specular_reflection);
}

vec3 SpecularIBL(Surface surface, MicrofacetMaterial material)
{
	vec3 result = vec3(0.0);
	 const uint SAMPLE_COUNT = 124u;

	Surface sample_surface;
	sample_surface.view = surface.view;
	sample_surface.normal = surface.normal;
	sample_surface.dotNV = max(0.0f, dot(sample_surface.normal, sample_surface.view));

	for(uint i = 0u; i < SAMPLE_COUNT; ++i)
	{
		vec2 Xi = Hammersley(i, SAMPLE_COUNT);
		sample_surface.halfVector = ImportanceSamplingGGX(Xi, surface.normal, material.roughness);
		sample_surface.light = -reflect(sample_surface.view, sample_surface.halfVector);

		sample_surface.dotNH = max(0.0f, dot(sample_surface.normal, sample_surface.halfVector));
		sample_surface.dotNL = max(0.0f, dot(sample_surface.normal, sample_surface.light));
		sample_surface.dotHV = max(0.0f, dot(sample_surface.halfVector, sample_surface.view));

		if(sample_surface.dotNL > 0)
        {
			vec3 F = F_Shlick(sample_surface, material.f0);
			float G = G_SmithGGX(surface, material.roughness);

			vec3 color = texture(environmentSampler, sample_surface.light).rgb;
	
			result += color * F * G * sample_surface.dotHV / (sample_surface.dotNH * sample_surface.dotNV);
        }
	}

	return result / float(SAMPLE_COUNT);
}

void main() {
	vec3 lightPos = ubo.cameraPos;
	vec3 lightDirWS = normalize(lightPos - fragPositionWS);
	vec3 cameraDirWS = normalize(ubo.cameraPos - fragPositionWS);

	vec3 normal = texture(normalSampler, fragTexCoord).xyz * 2.0f - vec3(1.0f, 1.0f, 1.0f);

	mat3 m;
	m[0] = normalize(fragTangentWS);
	m[1] = normalize(fragBinormalWS);
	m[2] = normalize(fragNormalWS);

	Surface surface;
	surface.light = lightDirWS;
	surface.view = cameraDirWS;
	surface.normal = normalize(m * normal);
	surface.halfVector = normalize(lightDirWS + cameraDirWS);
	surface.dotNH = max(0.0f, dot(surface.normal, surface.halfVector));
	surface.dotNL = max(0.0f, dot(surface.normal, surface.light));
	surface.dotNV = max(0.0f, dot(surface.normal, surface.view));
	surface.dotHV = max(0.0f, dot(surface.halfVector, surface.view));

	Surface ibl;
	ibl.light = reflect(-surface.view, surface.normal);
	ibl.view = cameraDirWS;
	ibl.normal = normalize(m * normal);
	ibl.halfVector = normalize(lightDirWS + cameraDirWS);
	ibl.dotNH = max(0.0f, dot(ibl.normal, ibl.halfVector));
	ibl.dotNL = max(0.0f, dot(ibl.normal, ibl.light));
	ibl.dotNV = max(0.0f, dot(ibl.normal, ibl.view));
	ibl.dotHV = max(0.0f, dot(ibl.halfVector, ibl.view));

	MicrofacetMaterial microfacet_material;
	microfacet_material.albedo = texture(albedoSampler, fragTexCoord).rgb;
	microfacet_material.roughness = texture(shadingSampler, fragTexCoord).g;
	microfacet_material.metalness = texture(shadingSampler, fragTexCoord).b;
	microfacet_material.f0 = lerp(vec3(0.04f), microfacet_material.albedo, microfacet_material.metalness);

	microfacet_material.albedo = lerp(microfacet_material.albedo, vec3(0.5f, 0.5f, 0.5f), ubo.lerpUserValues);
	microfacet_material.roughness = lerp(microfacet_material.roughness, ubo.userRoughness, ubo.lerpUserValues);
	microfacet_material.metalness = lerp(microfacet_material.metalness, ubo.userMetalness, ubo.lerpUserValues);

	// Direct light
	float attenuation = 1.0f / dot(lightPos - fragPositionWS, lightPos - fragPositionWS);

	vec3 light = MicrofacetBRDF(surface, microfacet_material) * attenuation * 2.0f * surface.dotNL;
	
	// Ambient light (IBL)
	vec3 ibl_diffuse  = texture(diffuseIrradianceSampler, ibl.normal).rgb * microfacet_material.albedo;
	ibl_diffuse  *= (1.0f - F_Shlick(ibl.dotNV, microfacet_material.f0, microfacet_material.roughness));

	vec3 ibl_specular = SpecularIBL(ibl, microfacet_material);

	vec3 ambient = ibl_diffuse * iPI + ibl_specular;

	ambient *= texture(aoSampler, fragTexCoord).r;

	// Result
	vec3 color = vec3(0.0f);
	color += ambient;
	//color += light;
	//color += texture(emissionSampler, fragTexCoord).rgb;

	// Tonemapping + gamma correction
	color = color / (color + vec3(1.0));
	color = pow(color, vec3(1.0/2.2));

	outColor = vec4(color, 1.0f);
	//outColor = vec4(1.0f);
}