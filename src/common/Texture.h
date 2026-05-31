#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "vgl.h"

class Texture
{
public:
    Texture() = default;
    ~Texture();

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    Texture(Texture&& other) noexcept;
    Texture& operator=(Texture&& other) noexcept;

    bool LoadFromFile(const char* filename,
                      bool flipVertically = true,
                      bool generateMipmaps = true);

    void Bind(GLuint unit = 0) const;
    void Destroy();

    GLuint GetHandle() const { return m_handle; }
    GLenum GetTarget() const { return m_target; }
    GLsizei GetWidth() const { return m_width; }
    GLsizei GetHeight() const { return m_height; }
    bool IsLoaded() const { return m_handle != 0; }

private:
    GLuint m_handle = 0;
    GLenum m_target = GL_TEXTURE_2D;
    GLsizei m_width = 0;
    GLsizei m_height = 0;
};

#endif /* __TEXTURE_H__ */
