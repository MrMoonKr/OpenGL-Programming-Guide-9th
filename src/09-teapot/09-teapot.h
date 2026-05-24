#ifndef __09_TEAPOT_H__
#define __09_TEAPOT_H__

#include "vapp.h"

class TeapotExample : public VermilionApplication
{
private:
    float aspect;
public:
    bool OnInitialize(const AppConfig& config) override;

    void OnDisplay() override;

    void OnShutdown() override;

    void OnResize(int width, int height) override;

    void OnKey(int key, int scancode, int action, int mods) override;
};

#endif /* __09_TEAPOT_H__ */
