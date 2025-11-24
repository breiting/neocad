#include "neocad/ui/Window.hpp"

#include <iostream>

using namespace nc;

bool Window::Create(const CreateInfo& ci) {
    if (!glfwInit()) {
        std::cerr << "GLFW init failed.\n";
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
        std::cerr << "Failed to create GLFW window.\n";
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(m_Window);
    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
        std::cerr << "Failed to init GLAD.\n";
        return false;
    }

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

bool Window::PollEvents() {
    if (!m_Window || glfwWindowShouldClose(m_Window))
        return false;
    glfwPollEvents();
    return true;
}

void Window::BeginFrame() {
    glClearColor(0.18f, 0.20f, 0.25f, 1.0f);  // Nord0
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::EndFrame() {
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
