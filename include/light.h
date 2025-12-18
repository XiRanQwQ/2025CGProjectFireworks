#ifndef POINT_LIGHT_H
#define POINT_LIGHT_H

struct PointLight {
    alignas(16) glm::vec3 position ;
    alignas(16) glm::vec3 color;   // rgb * intensity
    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.0002f;
    float  active = false;
};
#endif // !1

#include <glm/glm.hpp>

