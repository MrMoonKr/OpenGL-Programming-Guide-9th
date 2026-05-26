#ifndef __VAPP_H__
#define __VAPP_H__

#include <chrono>

#include "vgl.h"

struct AppConfig
{
    int width = 1280;
    int height = 720;
    const char* title = "App";

    int gl_major = 4;
    int gl_minor = 6;

    bool resizable = true;
    bool visible = true;
    bool decorated = true;
    bool maximized = false;
    bool vsync = false;
    bool scaleToMonitor = true;
    bool debugContext = true;
    bool highDpiFramebuffer = true;
};

class VermilionApplication
{
protected:
    GLFWwindow* m_pWindow = nullptr;
    std::chrono::steady_clock::time_point m_appStartTime{};
    std::chrono::steady_clock::time_point m_lastFrameTime{};
    AppConfig m_appConfig{};
    float m_FPS = 0.0f;
    float m_PrintInterval = 2.0f;
    float m_FpsAccumulatedTime = 0.0f;
    unsigned int m_FrameCount = 0;

public:
    VermilionApplication() = default;
    virtual ~VermilionApplication() = default;

    int Run(const AppConfig& startConfig);

protected:
    virtual bool OnInitialize(const AppConfig& config);
    virtual void OnShutdown();
    virtual void OnUpdate(float deltaTime) { /* NOTHING */ }

    virtual void OnDisplay()
    {
        glfwSwapBuffers(m_pWindow);
    }

    virtual void OnResize(int width, int height)
    {
        glViewport(0, 0, width, height);
    }

    virtual void OnKey(int key, int scancode, int action, int mods) { /* NOTHING */ }
    virtual void OnChar(unsigned int codepoint) { /* NOTHING */ }
    virtual void OnMouseButton(int button, int action, int mods) { /* NOTHING */ }
    virtual void OnCursorMove(double x, double y) { /* NOTHING */ }

    GLFWwindow* GetWindow() const { return m_pWindow; }
    const AppConfig& GetAppConfig() const { return m_appConfig; }
    unsigned int GetAppTime() const;
    float GetFPS() const { return m_FPS; }

private:
    void MainLoop();
    bool CreateAppWindow(const AppConfig& config);
    void DestroyAppWindow();

    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void char_callback(GLFWwindow* window, unsigned int codepoint);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void cursor_position_callback(GLFWwindow* window, double x, double y);

#ifdef _DEBUG
    static void APIENTRY DebugOutputCallback(GLenum source,
                                             GLenum type,
                                             GLuint id,
                                             GLenum severity,
                                             GLsizei length,
                                             const GLchar* message,
                                             const void* userParam);
#endif
};

#endif /* __VAPP_H__ */
