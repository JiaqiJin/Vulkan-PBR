#ifndef BRDF_H_
#define BRDF_H_

const float PI =  3.141592653589798979f;
const float PI2 = 6.283185307179586477f;
const float iPI = 0.318309886183790672f;

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

vec3 lerp(vec3 a, vec3 b, vec3 t)
{
	return a * (1.0f - t) + b * t;
}

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

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
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

#endif