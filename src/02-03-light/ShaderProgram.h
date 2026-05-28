#ifndef __02_03_LIGHT_SHADERPROGRAM_H__
#define __02_03_LIGHT_SHADERPROGRAM_H__

#include <string>
#include <unordered_map>

#include "Shader.h"
#include "vmath.h"

class ShaderProgram
{
private:
    GLuint m_id = 0;
    bool m_linked = false;
    std::unordered_map<std::string, GLint> m_uniformLocations {};

public:
    ShaderProgram() = default;
    ~ShaderProgram();

    ShaderProgram(const ShaderProgram&) = delete;
    ShaderProgram& operator=(const ShaderProgram&) = delete;

    ShaderProgram(ShaderProgram&& other) noexcept;
    ShaderProgram& operator=(ShaderProgram&& other) noexcept;

    bool Init(const char* vertSource, const char* fragSource);
    bool InitFromFiles(const char* vertPath, const char* fragPath);

    bool Attach(const Shader& shader);
    bool Link();
    void Use() const;
    void Destroy();

    GLint GetUniformLocation(const char* name);
    void SetMat4(const char* name, const vmath::mat4& value);
    void SetVec3(const char* name, const vmath::vec3& value);
    void SetFloat(const char* name, float value);
    void SetInt(const char* name, int value);

    GLuint GetId() const { return m_id; }
    bool IsLinked() const { return m_linked; }
};

#endif /* __02_03_LIGHT_SHADERPROGRAM_H__ */
