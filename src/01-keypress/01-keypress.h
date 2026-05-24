#ifndef __01_KEYPRESS_H__
#define __01_KEYPRESS_H__

#include "vapp.h"

class KeyPressExample : public VermilionApplication
{
public:
    bool OnInitialize(const AppConfig& config) override;

    void OnDisplay() override;

    void OnShutdown() override;

    void OnResize(int width, int height) override;

    void OnKey(int key, int scancode, int action, int mods) override;
};

#endif /* __01_KEYPRESS_H__ */
