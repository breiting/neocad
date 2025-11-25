#include <GLFW/glfw3.h>

#include <glm/gtx/matrix_interpolation.hpp>
#include <neocad/vis/ViewController.hpp>

using namespace nc::editor;

namespace nc::vis {

void ViewController::SetViewportSize(int w, int h) {
    m_W = w;
    m_H = h;
}

void ViewController::OnInput(const InputEvent& ev) {
    // 1) View-Mode-Switching
    if (ev.type == InputEventType::Key) {
        const auto& k = std::get<KeyEvent>(ev.data);
        if ((int)k.code == GLFW_KEY_KP_0)
            StartTransition(ViewMode::View3D);
        if ((int)k.code == GLFW_KEY_KP_1)
            StartTransition(ViewMode::Sketch2D);
        return;
    }

    // 2) Mouse/Scroll only
    if (m_Transition)
        return;  // ignore during animation

    // if (ev.type == nc::InputEventType::MouseButton || ev.type == nc::InputEventType::MouseMove ||
    //     ev.type == nc::InputEventType::Scroll) {
    //     HandleMouseInput(std::get_if<MouseButtonEvent>(&ev.data), std::get_if<nc::MouseMoveEvent>(&ev.data),
    //                      std::get_if<ScrollEvent>(&ev.data));
    // }
}

void ViewController::StartTransition(ViewMode target) {
    if (target == m_Mode)
        return;
    m_TargetMode = target;
    m_Transition = true;
    m_T = 0.0;
}

void ViewController::Update(double dt) {
    if (!m_Transition)
        return;

    m_T += dt / m_Duration;
    if (m_T >= 1.0) {
        m_T = 1.0;
        m_Transition = false;
        m_Mode = m_TargetMode;
    }
}

ViewState ViewController::GetViewState() {
    double aspect = double(m_W) / double(m_H);
    m_Cam2D.SetAspect(aspect);
    m_Cam3D.SetAspect(aspect);
    ViewState vs;

    if (!m_Transition) {
        vs.mode = m_Mode;
        if (m_Mode == ViewMode::View3D) {
            vs.view = m_Cam3D.View();
            vs.proj = m_Cam3D.Projection();
        } else {
            vs.view = m_Cam2D.View();
            vs.proj = m_Cam2D.Projection();
        }
        return vs;
    }

    // During transition → interpolate view matrix
    glm::dmat4 A = (m_Mode == ViewMode::View3D) ? m_Cam3D.View() : m_Cam2D.View();
    glm::dmat4 B = (m_TargetMode == ViewMode::View3D) ? m_Cam3D.View() : m_Cam2D.View();

    vs.view = glm::interpolate(A, B, m_T);

    if (m_TargetMode == ViewMode::View3D)
        vs.proj = m_Cam3D.Projection();
    else
        vs.proj = m_Cam2D.Projection();

    vs.mode = m_Mode;
    return vs;
}

}  // namespace nc::vis
