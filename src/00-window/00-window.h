#ifndef __00_WINDOW_H__
#define __00_WINDOW_H__

#include "vapp.h"

class WindowExample : public VermilionApplication
{
private:
    float m_time = 0.0f;

    float m_clearRed = 0.0f;

    float m_clearGreen = 0.0f;

    float m_clearBlue = 0.0f;
public:
    bool OnInitialize(const AppConfig& config) override;

    void OnUpdate(float deltaTime) override;

    void OnDisplay() override;

    void OnResize(int width, int height) override;
};

#endif /* __00_WINDOW_H__ */
