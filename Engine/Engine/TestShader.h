#include <glm/glm.hpp>
#define vec3 glm::vec3
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

#define HEIGHT_RAY 8e3

vec3 CalculateScattering(
    vec3 dir,  //camera direction
    vec3 light_dir, // the direction of the light
);
{
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
