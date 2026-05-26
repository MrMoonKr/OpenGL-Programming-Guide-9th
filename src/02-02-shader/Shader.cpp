#include "Shader.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>

namespace
{
bool ReadTextFile(const char* path, std::string& output)
{
    std::ifstream file(path, std::ios::binary);
    if (!file)
    {
        std::cerr << "Unable to open shader file '" << path << "'" << std::endl;
        return false;
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();
    output = buffer.str();
    return true;
}
} // namespace

Shader::~Shader()
{
    Destroy();
}

Shader::Shader(Shader&& other) noexcept
    : m_id(other.m_id)
    , m_type(other.m_type)
    , m_compiled(other.m_compiled)
    , m_label(std::move(other.m_label))
{
    other.m_id = 0;
    other.m_type = GL_NONE;
    other.m_compiled = false;
}

Shader& Shader::operator=(Shader&& other) noexcept
{
    if (this != &other)
    {
        Destroy();

        m_id = other.m_id;
        m_type = other.m_type;
        m_compiled = other.m_compiled;
        m_label = std::move(other.m_label);

        other.m_id = 0;
        other.m_type = GL_NONE;
        other.m_compiled = false;
    }

    return *this;
}

bool Shader::InitFromSource(GLenum type, const char* source, const char* label)
{
    Destroy();

    if (source == nullptr)
    {
        return false;
    }

    m_type = type;
    m_label = (label != nullptr) ? label : "";

    m_id = glCreateShader(type);
    glShaderSource(m_id, 1, &source, nullptr);
    glCompileShader(m_id);

    GLint compiled = GL_FALSE;
    glGetShaderiv(m_id, GL_COMPILE_STATUS, &compiled);
    if (compiled == GL_FALSE)
    {
        GLint logLength = 0;
        glGetShaderiv(m_id, GL_INFO_LOG_LENGTH, &logLength);

        std::string log(static_cast<size_t>(logLength > 0 ? logLength : 1), '\0');
        glGetShaderInfoLog(m_id,
                           static_cast<GLsizei>(log.size()),
                           nullptr,
                           log.data());

        std::cerr << "Shader compilation failed";
        if (!m_label.empty())
        {
            std::cerr << " for '" << m_label << "'";
        }
        std::cerr << ": " << log << std::endl;

        Destroy();
        return false;
    }

    m_compiled = true;
    return true;
}

bool Shader::InitFromFile(GLenum type, const char* path)
{
    std::string source;
    if (!ReadTextFile(path, source))
    {
        return false;
    }

    return InitFromSource(type, source.c_str(), path);
}

void Shader::Destroy()
{
    if (m_id != 0)
    {
        glDeleteShader(m_id);
        m_id = 0;
    }

    m_type = GL_NONE;
    m_compiled = false;
    m_label.clear();
}
