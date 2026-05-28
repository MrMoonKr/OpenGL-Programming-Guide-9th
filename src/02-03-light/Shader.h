#ifndef __02_03_LIGHT_SHADER_H__
#define __02_03_LIGHT_SHADER_H__

#include <string>

#include "vgl.h"

class Shader
{
private:
    GLuint m_id = 0;
    GLenum m_type = GL_NONE;
    bool m_compiled = false;
    std::string m_label {};

public:
    Shader() = default;
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

    bool InitFromSource(GLenum type, const char* source, const char* label = nullptr);
    bool InitFromFile(GLenum type, const char* path);
    void Destroy();

    GLuint GetId() const { return m_id; }
    GLenum GetType() const { return m_type; }
    bool IsCompiled() const { return m_compiled; }
    const std::string& GetLabel() const { return m_label; }
};

#endif /* __02_03_LIGHT_SHADER_H__ */
