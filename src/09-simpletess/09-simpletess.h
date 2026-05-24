#ifndef __09_SIMPLETESS_H__
#define __09_SIMPLETESS_H__

#include "vapp.h"

class SimpleTssellationExample : public VermilionApplication
{
private:
    float aspect;
public:
    bool OnInitialize(const AppConfig& config) override;

    void OnDisplay() override;

    void OnShutdown() override;

    void OnKey(int key, int scancode, int action, int mods) override;

    void OnResize(int width, int height) override;
};

#endif /* __09_SIMPLETESS_H__ */
