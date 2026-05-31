#include "Light.h"

#include <string>

namespace
{
std::string UniformName(const char* prefix, const char* suffix)
{
    std::string name(prefix != nullptr ? prefix : "");
    name += ".";
    name += suffix;
    return name;
}
} // namespace

void SunLight::Apply(ShaderProgram& program, const char* uniformPrefix) const
{
    program.SetInt(UniformName(uniformPrefix, "enabled").c_str(), enabled ? 1 : 0);
    program.SetVec3(UniformName(uniformPrefix, "direction").c_str(), direction);
    program.SetVec3(UniformName(uniformPrefix, "ambient").c_str(), ambient);
    program.SetVec3(UniformName(uniformPrefix, "diffuse").c_str(), diffuse);
    program.SetVec3(UniformName(uniformPrefix, "specular").c_str(), specular);
}

void PointLight::Apply(ShaderProgram& program, const char* uniformPrefix) const
{
    program.SetInt(UniformName(uniformPrefix, "enabled").c_str(), enabled ? 1 : 0);
    program.SetVec3(UniformName(uniformPrefix, "position").c_str(), position);
    program.SetVec3(UniformName(uniformPrefix, "ambient").c_str(), ambient);
    program.SetVec3(UniformName(uniformPrefix, "diffuse").c_str(), diffuse);
    program.SetVec3(UniformName(uniformPrefix, "specular").c_str(), specular);
    program.SetFloat(UniformName(uniformPrefix, "constant").c_str(), constant);
    program.SetFloat(UniformName(uniformPrefix, "linear").c_str(), linear);
    program.SetFloat(UniformName(uniformPrefix, "quadratic").c_str(), quadratic);
}
