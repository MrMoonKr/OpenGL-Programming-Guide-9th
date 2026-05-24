#ifndef __01_CUBE_H__
#define __01_CUBE_H__

#include "vapp.h"

class CubeExample : public VermilionApplication
{
private:
    float m_aspect = 16.0f / 9.0f;

    float m_rotationTime = 0.0f;

    GLuint m_program = 0;

    GLuint m_vao = 0;

    GLuint m_vertexBuffer = 0;

    GLuint m_indexBuffer = 0;

    GLint m_mvpLocation = -1;
public:
    bool OnInitialize(const AppConfig& config) override;

    void OnUpdate(float deltaTime) override;

    void OnDisplay() override;

    void OnShutdown() override;

    void OnResize(int width, int height) override;
};

#endif /* __01_CUBE_H__ */
