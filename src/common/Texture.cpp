#include "Texture.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <string>
#include <utility>

#include "stb_image.h"
#include "vermilion.h"

extern "C" void vglLoadDDS(const char* filename, vglImageData* image);

namespace
{
std::string ToLower(std::string value)
{
    std::transform(value.begin(),
                   value.end(),
                   value.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return value;
}

std::string GetExtension(const char* filename)
{
    if (filename == nullptr)
    {
        return {};
    }

    const char* dot = std::strrchr(filename, '.');
    if (dot == nullptr)
    {
        return {};
    }

    return ToLower(dot);
}

GLint ComputeMipLevels(GLsizei width, GLsizei height)
{
    GLint levels = 1;
    GLsizei size = (width > height) ? width : height;
    while (size > 1)
    {
        size /= 2;
        ++levels;
    }

    return levels;
}

bool UploadImageData(GLuint texture, const vglImageData& image)
{
    GLubyte* ptr = reinterpret_cast<GLubyte*>(image.mip[0].data);

    glBindTexture(image.target, texture);

    switch (image.target)
    {
        case GL_TEXTURE_1D:
            glTexStorage1D(image.target,
                           image.mipLevels,
                           image.internalFormat,
                           image.mip[0].width);
            for (int level = 0; level < image.mipLevels; ++level)
            {
                glTexSubImage1D(GL_TEXTURE_1D,
                                level,
                                0,
                                image.mip[level].width,
                                image.format,
                                image.type,
                                image.mip[level].data);
            }
            break;
        case GL_TEXTURE_1D_ARRAY:
            glTexStorage2D(image.target,
                           image.mipLevels,
                           image.internalFormat,
                           image.mip[0].width,
                           image.slices);
            for (int level = 0; level < image.mipLevels; ++level)
            {
                glTexSubImage2D(GL_TEXTURE_1D,
                                level,
                                0,
                                0,
                                image.mip[level].width,
                                image.slices,
                                image.format,
                                image.type,
                                image.mip[level].data);
            }
            break;
        case GL_TEXTURE_2D:
            glTexStorage2D(image.target,
                           image.mipLevels,
                           image.internalFormat,
                           image.mip[0].width,
                           image.mip[0].height);
            for (int level = 0; level < image.mipLevels; ++level)
            {
                glTexSubImage2D(GL_TEXTURE_2D,
                                level,
                                0,
                                0,
                                image.mip[level].width,
                                image.mip[level].height,
                                image.format,
                                image.type,
                                image.mip[level].data);
            }
            break;
        case GL_TEXTURE_CUBE_MAP:
            for (int level = 0; level < image.mipLevels; ++level)
            {
                ptr = reinterpret_cast<GLubyte*>(image.mip[level].data);
                for (int face = 0; face < 6; ++face)
                {
                    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face,
                                 level,
                                 image.internalFormat,
                                 image.mip[level].width,
                                 image.mip[level].height,
                                 0,
                                 image.format,
                                 image.type,
                                 ptr + image.sliceStride * face);
                }
            }
            break;
        case GL_TEXTURE_2D_ARRAY:
            glTexStorage3D(image.target,
                           image.mipLevels,
                           image.internalFormat,
                           image.mip[0].width,
                           image.mip[0].height,
                           image.slices);
            for (int level = 0; level < image.mipLevels; ++level)
            {
                glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
                                level,
                                0,
                                0,
                                0,
                                image.mip[level].width,
                                image.mip[level].height,
                                image.slices,
                                image.format,
                                image.type,
                                image.mip[level].data);
            }
            break;
        case GL_TEXTURE_CUBE_MAP_ARRAY:
            glTexStorage3D(image.target,
                           image.mipLevels,
                           image.internalFormat,
                           image.mip[0].width,
                           image.mip[0].height,
                           image.slices);
            break;
        case GL_TEXTURE_3D:
            glTexStorage3D(image.target,
                           image.mipLevels,
                           image.internalFormat,
                           image.mip[0].width,
                           image.mip[0].height,
                           image.mip[0].depth);
            for (int level = 0; level < image.mipLevels; ++level)
            {
                glTexSubImage3D(GL_TEXTURE_3D,
                                level,
                                0,
                                0,
                                0,
                                image.mip[level].width,
                                image.mip[level].height,
                                image.mip[level].depth,
                                image.format,
                                image.type,
                                image.mip[level].data);
            }
            break;
        default:
            return false;
    }

    glTexParameteriv(image.target,
                     GL_TEXTURE_SWIZZLE_RGBA,
                     reinterpret_cast<const GLint*>(image.swizzle));

    return true;
}
} // namespace

Texture::~Texture()
{
    Destroy();
}

Texture::Texture(Texture&& other) noexcept
    : m_handle(std::exchange(other.m_handle, 0))
    , m_target(std::exchange(other.m_target, GL_TEXTURE_2D))
    , m_width(std::exchange(other.m_width, 0))
    , m_height(std::exchange(other.m_height, 0))
{
}

Texture& Texture::operator=(Texture&& other) noexcept
{
    if (this != &other)
    {
        Destroy();
        m_handle = std::exchange(other.m_handle, 0);
        m_target = std::exchange(other.m_target, GL_TEXTURE_2D);
        m_width = std::exchange(other.m_width, 0);
        m_height = std::exchange(other.m_height, 0);
    }

    return *this;
}

bool Texture::LoadFromFile(const char* filename, bool flipVertically, bool generateMipmaps)
{
    Destroy();

    if (filename == nullptr)
    {
        std::fprintf(stderr, "Texture::LoadFromFile failed: filename is null\n");
        return false;
    }

    const std::string extension = GetExtension(filename);

    if (extension == ".dds")
    {
        vglImageData image {};
        vglLoadDDS(filename, &image);

        if (image.target == GL_NONE || image.mip[0].data == nullptr)
        {
            std::fprintf(stderr, "Texture::LoadFromFile failed to load DDS: %s\n", filename);
            return false;
        }

        glGenTextures(1, &m_handle);
        if (!UploadImageData(m_handle, image))
        {
            std::fprintf(stderr, "Texture::LoadFromFile failed to upload DDS: %s\n", filename);
            vglUnloadImage(&image);
            Destroy();
            return false;
        }

        m_target = image.target;
        m_width = image.mip[0].width;
        m_height = image.mip[0].height;

        const bool hasMipmaps = image.mipLevels > 1;
        glTexParameteri(m_target,
                        GL_TEXTURE_MIN_FILTER,
                        hasMipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
        glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(m_target, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(m_target, GL_TEXTURE_WRAP_T, GL_REPEAT);

        vglUnloadImage(&image);
        return true;
    }

    stbi_set_flip_vertically_on_load(flipVertically ? 1 : 0);

    int width = 0;
    int height = 0;
    int channels = 0;

    const bool isHdr = stbi_is_hdr(filename) != 0;

    glGenTextures(1, &m_handle);
    m_target = GL_TEXTURE_2D;
    glBindTexture(m_target, m_handle);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    if (isHdr)
    {
        float* pixels = stbi_loadf(filename, &width, &height, &channels, STBI_rgb_alpha);
        if (pixels == nullptr)
        {
            std::fprintf(stderr,
                         "Texture::LoadFromFile failed to load HDR image %s: %s\n",
                         filename,
                         stbi_failure_reason());
            Destroy();
            return false;
        }

        const GLint mipLevels = generateMipmaps ? ComputeMipLevels(width, height) : 1;
        glTexStorage2D(GL_TEXTURE_2D, mipLevels, GL_RGBA32F, width, height);
        glTexSubImage2D(GL_TEXTURE_2D,
                        0,
                        0,
                        0,
                        width,
                        height,
                        GL_RGBA,
                        GL_FLOAT,
                        pixels);

        if (generateMipmaps)
        {
            glGenerateMipmap(GL_TEXTURE_2D);
        }

        stbi_image_free(pixels);
    }
    else
    {
        stbi_uc* pixels = stbi_load(filename, &width, &height, &channels, STBI_rgb_alpha);
        if (pixels == nullptr)
        {
            std::fprintf(stderr,
                         "Texture::LoadFromFile failed to load image %s: %s\n",
                         filename,
                         stbi_failure_reason());
            Destroy();
            return false;
        }

        const GLint mipLevels = generateMipmaps ? ComputeMipLevels(width, height) : 1;
        glTexStorage2D(GL_TEXTURE_2D, mipLevels, GL_RGBA8, width, height);
        glTexSubImage2D(GL_TEXTURE_2D,
                        0,
                        0,
                        0,
                        width,
                        height,
                        GL_RGBA,
                        GL_UNSIGNED_BYTE,
                        pixels);

        if (generateMipmaps)
        {
            glGenerateMipmap(GL_TEXTURE_2D);
        }

        stbi_image_free(pixels);
    }

    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_MIN_FILTER,
                    generateMipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    m_width = static_cast<GLsizei>(width);
    m_height = static_cast<GLsizei>(height);

    return true;
}

void Texture::Bind(GLuint unit) const
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(m_target, m_handle);
}

void Texture::Destroy()
{
    if (m_handle != 0)
    {
        glDeleteTextures(1, &m_handle);
        m_handle = 0;
    }

    m_target = GL_TEXTURE_2D;
    m_width = 0;
    m_height = 0;
}
