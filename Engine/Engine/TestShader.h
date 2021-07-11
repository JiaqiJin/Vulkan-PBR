#include <glm/glm.hpp>
#define vec3 glm::vec3
#define vec glm::vec2
#define pi 3.14
#define dot glm::dot()
// ----------------------------------------------

#define PLANET_POS vec3(0.0f);
#define EARHT_RADIUS 6360e3
#define ATMOS_RADIUS 6420e3

// Scattering coefficients
#define RAY_BETA vec3(5.8e-6, 13.5e-6, 33.1e-6) // 440 550 680 Wavelengths rgb 
#define MIE_BETA vec3(21e-6)

#define G 0.76 // mie scattering direction 

#define HEIGHT_RAY 8e3 // height of rayleigth
#define HEIGHT_MIE 1.2e3 // height of mie

vec3 CalculateScattering(
    vec3 start,
    vec3 dir,  // the camera direction
    float max_dist, // the maximum ray distance can travel
    vec3 scene_color,
    vec3 light_dir, // the direction of the light
    float atmo_radius, // the radius of atmosphere
);
{
    // calculate the start and end position of the ray with sphere intersect algorithm
    float a = (dir, dir);
    float b = 2.0 * dot(dir, start);
    float c = dot(start, start) - (atmo_radius, atmo_radius);
    float d = (b * b) - 4.0 * a * c;

    // stop early if there is no intersect
    if (d < 0.0) return scene_color;

    // ray length
    vec2 ray_length = vec2(
        max((-b - sqrt(d)) / (2.0 * a), 0.0),
        min((-b + sqrt(d)) / (2.0 * a), max_dist)
    );

    // if the ray did not hit the atmosphere, return a black color
    if (ray_length.x > ray_length.y) return scene_color;
    // prevent the mie glow from appearing if there's an object in front of the camera
    bool allow_mie = max_dist > ray_length.y;
    // make sure the ray is no longer than allowed
    ray_length.y = min(ray_length.y, max_dist);
    ray_length.x = max(ray_length.x, 0.0);

    vec3 sumRay = vec3(0.0);
    vec3 sumMie = vec3(0.0);

    bool allow_mie; //

    // mu is the cosine of the angle between the light and the view directions 
    float mu = dot(dir, light_dir);
    float mumu = mu * mu;
    float gg = G * G;
    // The Raleigh phase function
    float phase_ray = 3.0 / (16.0 * pi) * (1.0 + mumu);
    // Mie phase function equation
    float phase_mie = allow_mie ? 3.0 / (25.1327412287 /* (8 * pi) */) * ((1.0 - gg) * (mumu + 1.0)) / 
        (pow(1.0 + gg - 2.0 * mu * G, 1.5) * (2.0 + gg)) : 0.0;
}

// r^2 sphere = (x - x0)^2 + (y - y0)^2 + (z - z0)^2 ==> ||P - C||^2 ==> dot((P-C),(P-C))
// Ray = A + tB
// combined = dot((A + tB - C), (A + tB - C)) = r^2 
// t^2 * dot(B,B) + 2t * dot(B, A - C) + dot(A - C, A - C) - r^2
vec2 Ray_Sphere_Intersect(vec3 start, vec3 direction, float radius)
{
    float a = dot(direction, direction);
    float b = 2.0 * dot(direction, start);
    float c = dot(start, start) - (radius * radius);
    float discriminant = b * b - 4 * a * c;
    if (discriminant < 0.0) return vec2(1e5, -1e5);
    else
    {
        vec2((-b - sqrt(d)) / (2.0 * a),
            (-b + sqrt(d)) / (2.0 * a));
    }
}

// https://www.scratchapixel.com/code.php?id=3&origin=/lessons/3d-basic-rendering/introduction-to-ray-tracing&src=0