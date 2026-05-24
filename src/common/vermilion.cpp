#include "vapp.h"

#include <chrono>
#include <cstdio>

#ifdef _DEBUG
namespace
{
void PrintDebugMessage(const char* message)
{
#ifdef _WIN32
    OutputDebugStringA(message);
#else
    std::fputs(message, stderr);
#endif
}
} // namespace
#endif

void VermilionApplication::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    VermilionApplication* pThis = (VermilionApplication*)glfwGetWindowUserPointer(window);

    pThis->OnResize(width, height);
}

void VermilionApplication::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    VermilionApplication* pThis = (VermilionApplication*)glfwGetWindowUserPointer(window);

    pThis->OnKey(key, scancode, action, mods);
}

void VermilionApplication::char_callback(GLFWwindow* window, unsigned int codepoint)
{
    VermilionApplication* pThis = (VermilionApplication*)glfwGetWindowUserPointer(window);

    pThis->OnChar(codepoint);
}

unsigned int VermilionApplication::GetAppTime() const
{
    const auto elapsed =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - m_appStartTime);

    return static_cast<unsigned int>(elapsed.count());
}

void VermilionApplication::MainLoop()
{
    while (m_pWindow != nullptr && !glfwWindowShouldClose(m_pWindow))
    {
        glfwPollEvents();

        const auto now = std::chrono::steady_clock::now();
        const auto elapsed =
            std::chrono::duration_cast<std::chrono::duration<float>>(now - m_lastFrameTime);
        float deltaTime = elapsed.count();
        if (deltaTime > 0.1f)
        {
            deltaTime = 0.1f;
        }
        m_lastFrameTime = now;

        OnUpdate(deltaTime);
        OnDisplay();

#ifdef _DEBUG
        ++m_FrameCount;
        m_FpsAccumulatedTime += deltaTime;
        if (m_FpsAccumulatedTime >= m_PrintInterval)
        {
            m_FPS = static_cast<float>(m_FrameCount) / m_FpsAccumulatedTime;

            char buffer[128];
            std::snprintf(buffer,
                          sizeof(buffer),
                          "[Vermilion] FPS: %.2f (avg %.2fs)\n",
                          m_FPS,
                          m_FpsAccumulatedTime);
            PrintDebugMessage(buffer);

            m_FpsAccumulatedTime = 0.0f;
            m_FrameCount = 0;
        }
#endif
    }
}

int VermilionApplication::Run(const AppConfig& startConfig)
{
    m_appConfig = startConfig;

    if (!OnInitialize(m_appConfig))
    {
        OnShutdown();
        DestroyAppWindow();
        glfwTerminate();
        return -1;
    }

    m_lastFrameTime = std::chrono::steady_clock::now();
    m_FPS = 0.0f;
    m_FpsAccumulatedTime = 0.0f;
    m_FrameCount = 0;
    MainLoop();
    OnShutdown();
    DestroyAppWindow();
    glfwTerminate();

    return 0;
}

#ifdef _DEBUG
void APIENTRY VermilionApplication::DebugOutputCallback(GLenum source,
                                                        GLenum type,
                                                        GLuint id,
                                                        GLenum severity,
                                                        GLsizei length,
                                                        const GLchar* message,
                                                        const void* userParam)
{
#ifdef _WIN32
    (void)source;
    (void)type;
    (void)id;
    (void)severity;
    (void)length;
    (void)userParam;
    OutputDebugStringA(message);
    OutputDebugStringA("\n");
#else
    (void)userParam;
    std::fprintf(stderr,
                 "Debug Message: SOURCE(0x%04X), TYPE(0x%04X), ID(0x%08X), SEVERITY(0x%04X), \"%s\"\n",
                 source,
                 type,
                 id,
                 severity,
                 message);
#endif
}
#endif

bool VermilionApplication::CreateAppWindow(const AppConfig& config)
{
    glfwDefaultWindowHints();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, config.gl_major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, config.gl_minor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, config.resizable ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_VISIBLE, config.visible ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_DECORATED, config.decorated ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_MAXIMIZED, config.maximized ? GLFW_TRUE : GLFW_FALSE);
#ifdef GLFW_SCALE_TO_MONITOR
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, config.scaleToMonitor ? GLFW_TRUE : GLFW_FALSE);
#endif
#ifdef GLFW_COCOA_RETINA_FRAMEBUFFER
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, config.highDpiFramebuffer ? GLFW_TRUE : GLFW_FALSE);
#endif
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

#ifdef _DEBUG
    if (config.debugContext)
    {
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    }
#endif

    m_pWindow = glfwCreateWindow(config.width,
                                 config.height,
                                 config.title != nullptr ? config.title : "App",
                                 nullptr,
                                 nullptr);
    if (m_pWindow == nullptr)
    {
        return false;
    }

    glfwSetWindowUserPointer(m_pWindow, this);
    glfwSetFramebufferSizeCallback(m_pWindow, framebuffer_size_callback);
    glfwSetKeyCallback(m_pWindow, key_callback);
    glfwSetCharCallback(m_pWindow, char_callback);

    glfwMakeContextCurrent(m_pWindow);
    glfwSwapInterval(config.vsync ? 1 : 0);

    if (gladLoadGL(glfwGetProcAddress) == 0)
    {
        DestroyAppWindow();
        return false;
    }

    int framebufferWidth = 0;
    int framebufferHeight = 0;
    glfwGetFramebufferSize(m_pWindow, &framebufferWidth, &framebufferHeight);
    OnResize(framebufferWidth, framebufferHeight);

#ifdef _DEBUG
    if (config.debugContext && GLAD_GL_VERSION_4_3)
    {
        glDebugMessageCallback(DebugOutputCallback, this);
    }
#endif

    return true;
}

void VermilionApplication::DestroyAppWindow()
{
    if (m_pWindow != nullptr)
    {
        glfwDestroyWindow(m_pWindow);
        m_pWindow = nullptr;
    }
}

bool VermilionApplication::OnInitialize(const AppConfig& config)
{
    m_appStartTime = std::chrono::steady_clock::now();

    if (!glfwInit())
    {
        return false;
    }

    return CreateAppWindow(config);
}

void VermilionApplication::OnShutdown()
{
}
