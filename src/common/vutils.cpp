#include "vutils.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace
{
bool CompileAndAttachShader(GLuint prog, GLenum type, const char* source, const char* label)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint compiled = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (compiled == GL_FALSE)
    {
        GLint log_length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);

        std::string log(static_cast<size_t>(log_length > 0 ? log_length : 1), '\0');
        glGetShaderInfoLog(shader,
                           static_cast<GLsizei>(log.size()),
                           nullptr,
                           log.data());

        std::cerr << "Shader compilation failed";
        if (label != nullptr)
        {
            std::cerr << " for '" << label << "'";
        }
        std::cerr << ": " << log << std::endl;

        glDeleteShader(shader);
        return false;
    }

    glAttachShader(prog, shader);
    glDeleteShader(shader);
    return true;
}

bool ReadTextFile(const char* filename, std::string& output)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file)
    {
        std::cerr << "Unable to open shader file '" << filename << "'" << std::endl;
        return false;
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();
    output = buffer.str();
    return true;
}
} // namespace

bool vglAttachShaderSource(GLuint prog, GLenum type, const char* source)
{
    if (source == nullptr)
    {
        return false;
    }

    return CompileAndAttachShader(prog, type, source, nullptr);
}

bool vglAttachShaderFile(GLuint prog, GLenum type, const char* filename)
{
    std::string source;
    if (!ReadTextFile(filename, source))
    {
        return false;
    }

    return CompileAndAttachShader(prog, type, source.c_str(), filename);
}
