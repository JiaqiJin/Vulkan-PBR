#version 450
#extension GL_ARB_separate_shader_objects : enable
#pragma shader_stage(fragment)

layout(set = 0, binding = 0) uniform UniformBufferObject {
	mat4 world;
	mat4 view;
	mat4 proj;
	vec3 cameraPos;
	float lerpUserValues;
	float userMetalness;
	float userRoughness;
} ubo;

layout(set = 1, binding = 1) uniform sampler2D albedoSampler;
layout(set = 1, binding = 2) uniform sampler2D normalSampler;
layout(set = 1, binding = 3) uniform sampler2D aoSampler;
layout(set = 1, binding = 4) uniform sampler2D shadingSampler;
layout(set = 1, binding = 5) uniform sampler2D emissionSampler;
layout(set = 1, binding = 6) uniform samplerCube hdrSampler;
layout(set = 1, binding = 7) uniform samplerCube diffuseIrradianceSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragTangentWS;
layout(location = 3) in vec3 fragBinormalWS;
layout(location = 4) in vec3 fragNormalWS;
layout(location = 5) in vec3 fragPositionWS;

layout(location = 0) out vec4 outColor;

const float PI = 3.141592653589798979f;
const float PI2 = pow(PI, 2.0f);
const float iPI = 0.31830988618379f;

float sqr(float a)
{
	return a * a;
}

float lerp(float a, float b, float t)
{
	return a * (1.0f - t) + b * t;
}

vec3 lerp(vec3 a, vec3 b, float t)
{
	return a * (1.0f - t) + b * t;
}

struct Surface
{
	vec3 light;
	vec3 view;
	vec3 normal;
	vec3 halfVector;
	float dotNH;
	float dotNL;
	float dotNV;
	float dotHV;
};

// ---------- PBR ---------------------

struct MicrofacetMaterial
{
	vec3 albedo;
	float roughness;
	float metalness;
	vec3 f0;
};

vec3 ImportanceSamplingGGX(vec2 Xi, vec3 normal, float roughness)
{
	float alpha = sqr(roughness * roughness);
	float alpha2 = sqr(alpha);

	float phi = PI2 * Xi.x;
	float cosTheta = sqrt((1.0f - Xi.y) / (1.0f + (alpha2 - 1.0f) * Xi.y));
	float sinTheta = sqrt(1.0f - sqr(cosTheta));

	// from spherical coordinates to cartesian coordinates
	vec3 H;
	H.x = cos(phi) * sinTheta;
	H.y = sin(phi) * sinTheta;
	H.z = cosTheta;

	// from tangent-space vector to world-space sample vector
	vec3 up        = abs(normal.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
	vec3 tangent   = normalize(cross(up, normal));
	vec3 bitangent = cross(normal, tangent);

	vec3 sampleVec = tangent * H.x + bitangent * H.y + normal * H.z;
	return normalize(sampleVec);
}

// Low Discrepancy
float RadicalInverse_VdC(uint bits) 
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

vec2 Hammersley(uint i, uint N)
{
    return vec2(float(i)/float(N), RadicalInverse_VdC(i));
}  

// relative surface area of microfacete aligned with to the half vector h
// D = alpha^2 / pi((n · h)^2 (alpha^2 - 1) + 1)^2

float DistributionGGX(Surface surface, float roughness)
{
	float alpha2 = roughness * roughness;
	float dotNH = dot(surface.normal, surface.halfVector);

	return iPI * alpha2 / sqr(1.0f + dotNH * dotNH * (alpha2 - 1.0f));
}

// Geometry function approximates the relative surface area where it micro surface-detail overshadow each other
// G = n · v / (n · v) (1 - k) + k
// G(n,v,l,k) = Gsub(n,v,k) Gsub(n,l,k)

float GeometrySchlickGGX(float NdotV, float roughness)
{
	float nom = NdotV;
	float denom = NdotV * (1.0f - roughness) + roughness;

	return nom / denom;
}

float G_SmithGGX(Surface surface, float roughness)
{
	float NdotL = max(dot(surface.normal, surface.light), 0.0f);
	float NdotV = max(dot(surface.normal, surface.view), 0.0f);

	float ggx1 = GeometrySchlickGGX(NdotV, roughness);
    float ggx2 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}

// Fresnel equation describe ratio of ligth that get reflected over the light get refracted
// which varies over all angle we`re looking at a surface

vec3 F_Shlick(Surface surface, vec3 f0)
{
	// cosTheta = dot(normal, halfVector)
	float dotHV = max(dot(surface.halfVector, surface.view), 0.0f);

	return f0 + (vec3(1.0f, 1.0f, 1.0f) - f0) * pow(1.0f - dotHV, 5);
}

vec3 F_Shlick(float cosTheta, vec3 f0, float roughness)
{
	return f0 + (max(vec3(1.0 - roughness), f0) - f0) * pow(1.0f - cosTheta, 5);
}

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

			vec3 color = texture(hdrSampler, sample_surface.light).rgb;
	
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
}