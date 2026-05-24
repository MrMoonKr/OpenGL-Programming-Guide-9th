#ifndef __06_LOAD_TEXTURE_H__
#define __06_LOAD_TEXTURE_H__

#include "vapp.h"

class LoadTextureExample : public VermilionApplication
{
private:
    // Member variables
    float aspect;

    GLuint base_prog;

    GLuint vao;

    GLuint quad_vbo;

    GLuint tex;
public:
    // Override functions from base class
    bool OnInitialize(const AppConfig& config) override;

    void OnDisplay() override;

    void OnShutdown() override;
private:
    virtual void Reshape(int width, int height);
};

#endif /* __06_LOAD_TEXTURE_H__ */
