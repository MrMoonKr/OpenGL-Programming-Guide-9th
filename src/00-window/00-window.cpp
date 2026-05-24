//////////////////////////////////////////////////////////////////////////////
//
//  00-window.cpp
//
//////////////////////////////////////////////////////////////////////////////

#include "00-window.h"

#include <cmath>

bool WindowExample::OnInitialize(const AppConfig& config)
{
    if (!VermilionApplication::OnInitialize(config))
    {
        return false;
    }

    glDisable(GL_DEPTH_TEST);
    return true;
}

void WindowExample::OnUpdate(float deltaTime)
{
    m_time += deltaTime;
    m_clearRed = 0.5f + 0.5f * std::sinf(m_time * 0.65f);
    m_clearGreen = 0.5f + 0.5f * std::sinf(m_time * 0.83f + 1.2f);
    m_clearBlue = 0.5f + 0.5f * std::sinf(m_time * 0.47f + 2.4f);
}

void WindowExample::OnDisplay()
{
    glClearColor(m_clearRed, m_clearGreen, m_clearBlue, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    VermilionApplication::OnDisplay();
}

void WindowExample::OnResize(int width, int height)
{
    glViewport(0, 0, width, height);
}

int main(int argc, char** argv)
{
    AppConfig config{};
    config.title = "Application Framework Example";

    WindowExample app;
    return app.Run(config);
}
