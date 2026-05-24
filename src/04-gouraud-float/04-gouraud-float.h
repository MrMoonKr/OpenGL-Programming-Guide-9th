#ifndef __04_GOURAUD_FLOAT_H__
#define __04_GOURAUD_FLOAT_H__

#include "vapp.h"

class GouraudFloatExample : public VermilionApplication
{
public:
    bool OnInitialize(const AppConfig& config) override;

    void OnDisplay() override;

    void OnKey(int key, int scancode, int action, int mods) override;
};

#endif /* __04_GOURAUD_FLOAT_H__ */
