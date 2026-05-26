#include "ShaderProgram.h"

#include <iostream>
#include <string>
#include <utility>

ShaderProgram::~ShaderProgram()
{
    Destroy();
}

ShaderProgram::ShaderProgram(ShaderProgram&& other) noexcept
    : m_id(other.m_id)
    , m_linked(other.m_linked)
    , m_uniformLocations(std::move(other.m_uniformLocations))
{
    other.m_id = 0;
    other.m_linked = false;
}

ShaderProgram& ShaderProgram::operator=(ShaderProgram&& other) noexcept
{
    if (this != &other)
    {
        Destroy();

        m_id = other.m_id;
        m_linked = other.m_linked;
        m_uniformLocations = std::move(other.m_uniformLocations);

        other.m_id = 0;
        other.m_linked = false;
    }

    return *this;
}

bool ShaderProgram::Init(const char* vertSource, const char* fragSource)
{
    Destroy();

    Shader vertexShader;
    Shader fragmentShader;

    if (!vertexShader.InitFromSource(GL_VERTEX_SHADER, vertSource, "vertex shader") ||
        !fragmentShader.InitFromSource(GL_FRAGMENT_SHADER, fragSource, "fragment shader"))
    {
        return false;
    }

    if (!Attach(vertexShader) || !Attach(fragmentShader) || !Link())
    {
        Destroy();
        return false;
    }

    return true;
}

bool ShaderProgram::InitFromFiles(const char* vertPath, const char* fragPath)
{
    Destroy();

    Shader vertexShader;
    Shader fragmentShader;

    if (!vertexShader.InitFromFile(GL_VERTEX_SHADER, vertPath) ||
        !fragmentShader.InitFromFile(GL_FRAGMENT_SHADER, fragPath))
    {
        return false;
    }

    if (!Attach(vertexShader) || !Attach(fragmentShader) || !Link())
    {
        Destroy();
        return false;
    }

    return true;
}

bool ShaderProgram::Attach(const Shader& shader)
{
    if (!shader.IsCompiled())
    {
        return false;
    }

    if (m_id == 0)
    {
        m_id = glCreateProgram();
    }

    glAttachShader(m_id, shader.GetId());
    return true;
}

bool ShaderProgram::Link()
{
    if (m_id == 0)
    {
        return false;
    }

    glLinkProgram(m_id);

    GLint linked = GL_FALSE;
    glGetProgramiv(m_id, GL_LINK_STATUS, &linked);
    if (linked == GL_FALSE)
    {
        GLint logLength = 0;
        glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &logLength);

        std::string log(static_cast<size_t>(logLength > 0 ? logLength : 1), '\0');
        glGetProgramInfoLog(m_id,
                            static_cast<GLsizei>(log.size()),
                            nullptr,
                            log.data());

        std::cerr << "Program link failed: " << log << std::endl;
        m_linked = false;
        return false;
    }

    m_linked = true;
    m_uniformLocations.clear();
    return true;
}

void ShaderProgram::Use() const
{
    glUseProgram(m_id);
}

void ShaderProgram::Destroy()
{
    if (m_id != 0)
    {
        glDeleteProgram(m_id);
        m_id = 0;
    }

    m_linked = false;
    m_uniformLocations.clear();
}

GLint ShaderProgram::GetUniformLocation(const char* name)
{
    const auto found = m_uniformLocations.find(name);
    if (found != m_uniformLocations.end())
    {
        return found->second;
    }

    const GLint location = glGetUniformLocation(m_id, name);
    m_uniformLocations.emplace(name, location);
    return location;
}

void ShaderProgram::SetMat4(const char* name, const vmath::mat4& value)
{
    glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, value);
}

void ShaderProgram::SetVec3(const char* name, const vmath::vec3& value)
{
    glUniform3f(GetUniformLocation(name), value[0], value[1], value[2]);
}

void ShaderProgram::SetFloat(const char* name, float value)
{
    glUniform1f(GetUniformLocation(name), value);
}

void ShaderProgram::SetInt(const char* name, int value)
{
    glUniform1i(GetUniformLocation(name), value);
}
