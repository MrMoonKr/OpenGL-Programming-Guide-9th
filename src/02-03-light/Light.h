#ifndef __02_03_LIGHT_LIGHT_H__
#define __02_03_LIGHT_LIGHT_H__

#include "ShaderProgram.h"
#include "vmath.h"

class SunLight
{
public:
    bool enabled = true;
    vmath::vec3 direction { -0.6f, -1.0f, -0.35f };
    vmath::vec3 ambient { 0.15f, 0.15f, 0.18f };
    vmath::vec3 diffuse { 0.70f, 0.68f, 0.62f };
    vmath::vec3 specular { 0.50f, 0.48f, 0.45f };

    void Apply(ShaderProgram& program, const char* uniformPrefix) const;
};

class PointLight
{
public:
    bool enabled = true;
    vmath::vec3 position { 3.0f, 2.5f, 0.0f };
    vmath::vec3 ambient { 0.05f, 0.03f, 0.02f };
    vmath::vec3 diffuse { 1.00f, 0.72f, 0.38f };
    vmath::vec3 specular { 1.00f, 0.82f, 0.60f };
    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;

    void Apply(ShaderProgram& program, const char* uniformPrefix) const;
};

#endif /* __02_03_LIGHT_LIGHT_H__ */
