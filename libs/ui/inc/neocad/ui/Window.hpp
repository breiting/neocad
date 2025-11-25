#pragma once

// clang-format off
#include <glad.h>
#include <GLFW/glfw3.h>
// clang-format on

#include <functional>
#include <string>

namespace nc::ui {

class Window {
   public:
    struct CreateInfo {
        int width = 1280;
        int height = 720;
        std::string title = "neocad";
        int msaa = 4;
    };

    bool Create(const CreateInfo &ci);
    void Destroy();

    bool PollEvents();  // returns false if window should close
    void BeginFrame();  // clear screen
    void EndFrame();    // swap buffers

    float Aspect() const;
    int GetWidth() const {
        return m_FramebufferWidth;
    }
    int GetHeight() const {
        return m_FramebufferHeight;
    }

    void SetKeyPressedCallback(std::function<void(int key, int action)> cb);
    void SetMouseButtonCallback(std::function<void(int button, int action, int mods)> cb);
    void SetMouseMoveCallback(std::function<void(double x, double y)> cb);
    void SetScrollCallback(std::function<void(double dx, double dy)> cb);

    GLFWwindow *GetNative() {
        return m_Window;
    }

   private:
    void InitCallbacks();

    GLFWwindow *m_Window = nullptr;
    int m_FramebufferWidth = 0;
    int m_FramebufferHeight = 0;

    std::function<void(int, int)> m_KeyPressedCallback;
    std::function<void(int, int, int)> m_MouseButtonCallback;
    std::function<void(double, double)> m_MouseMoveCallback;
    std::function<void(double, double)> m_ScrollCallback;
};

}  // namespace nc
