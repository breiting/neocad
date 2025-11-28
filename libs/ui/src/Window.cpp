#include <neocad/core/Logger.hpp>
#include <neocad/ui/Window.hpp>

#include "GLFW/glfw3.h"

GLuint g_DefaultTexture = 0;

/**
 * Creates a default texture which can be used, if no textures are used for shaders (prevent warning)
 */
void CreateDefaultTexture() {
    unsigned char whitePixel[4] = {255, 255, 255, 255};  // RGBA white

    glGenTextures(1, &g_DefaultTexture);
    glBindTexture(GL_TEXTURE_2D, g_DefaultTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, whitePixel);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

namespace nc::ui {
bool Window::Create(const CreateInfo& ci) {
    if (!glfwInit()) {
        LOG(Error) << "GLFW init failed.\n";
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_SAMPLES, ci.msaa);

    m_Window = glfwCreateWindow(ci.width, ci.height, ci.title.c_str(), nullptr, nullptr);
    if (!m_Window) {
        LOG(Error) << "Failed to create GLFW window.\n";
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(m_Window);
    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
        LOG(Error) << "Failed to init GLAD.\n";
        return false;
    }

    CreateDefaultTexture();

    glfwSwapInterval(1);  // VSync
    InitCallbacks();

    glfwGetFramebufferSize(m_Window, &m_FramebufferWidth, &m_FramebufferHeight);
    glViewport(0, 0, m_FramebufferWidth, m_FramebufferHeight);

    return true;
}

void Window::Destroy() {
    if (m_Window)
        glfwDestroyWindow(m_Window);
    glfwTerminate();
}

void Window::Close() {
    glfwSetWindowShouldClose(m_Window, true);
}

bool Window::PollEvents() {
    if (!m_Window || glfwWindowShouldClose(m_Window))
        return false;
    glfwPollEvents();
    return true;
}

void Window::SwapBuffers() {
    glfwSwapBuffers(m_Window);
}

void Window::InitCallbacks() {
    glfwSetWindowUserPointer(m_Window, this);

    glfwSetKeyCallback(m_Window, [](GLFWwindow* w, int key, int /*sc*/, int action, int mods) {
        auto* self = static_cast<Window*>(glfwGetWindowUserPointer(w));
        if (!self || !self->m_KeyPressedCallback || action != GLFW_PRESS)
            return;
        self->m_KeyPressedCallback(key, mods);
    });
    glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* w, int button, int action, int mods) {
        auto* self = static_cast<Window*>(glfwGetWindowUserPointer(w));
        if (!self || !self->m_MouseButtonCallback)
            return;
        self->m_MouseButtonCallback(button, action, mods);
    });

    glfwSetScrollCallback(m_Window, [](GLFWwindow* w, double xoff, double yoff) {
        auto* self = static_cast<Window*>(glfwGetWindowUserPointer(w));
        if (!self || !self->m_ScrollCallback)
            return;
        self->m_ScrollCallback(xoff, yoff);
    });

    glfwSetCursorPosCallback(m_Window, [](GLFWwindow* w, double x, double y) {
        auto* self = static_cast<Window*>(glfwGetWindowUserPointer(w));
        if (!self || !self->m_MouseMoveCallback)
            return;
        self->m_MouseMoveCallback(x, y);
    });

    glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow* w, int width, int height) {
        auto* self = static_cast<Window*>(glfwGetWindowUserPointer(w));
        if (!self)
            return;
        self->m_WindowSizeCallback(width, height);
    });
}

float Window::Aspect() const {
    return (float)m_FramebufferWidth / (float)m_FramebufferHeight;
}

void Window::SetMouseButtonCallback(std::function<void(int, int, int)> cb) {
    m_MouseButtonCallback = std::move(cb);
}

void Window::SetMouseMoveCallback(std::function<void(double, double)> cb) {
    m_MouseMoveCallback = std::move(cb);
}

void Window::SetKeyPressedCallback(std::function<void(int, int)> cb) {
    m_KeyPressedCallback = cb;
}

void Window::SetScrollCallback(std::function<void(double, double)> cb) {
    m_ScrollCallback = std::move(cb);
}
void Window::SetWindowSizeCallback(std::function<void(int, int)> cb) {
    m_WindowSizeCallback = std::move(cb);
}
}  // namespace nc::ui
