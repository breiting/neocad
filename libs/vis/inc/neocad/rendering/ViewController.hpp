#pragma once
#include <neocad/editor/InputEvent.hpp>
#include <neocad/rendering/Camera2D.hpp>
#include <neocad/rendering/Camera3D.hpp>
#include <neocad/rendering/ViewMode.hpp>

namespace nc::vis {

struct ViewState {
    ViewMode mode;
    glm::dmat4 view;
    glm::dmat4 proj;
};

class ViewController {
   public:
    void SetViewportSize(int w, int h);

    void Update(double dt);
    void OnInput(const editor::InputEvent& ev);

    ViewState GetViewState();

   private:
    Camera3D m_Cam3D;
    Camera2D m_Cam2D;

    int m_W = 1280, m_H = 720;
    ViewMode m_Mode = ViewMode::View3D;

    // --- Transition ---
    bool m_Transition = false;
    ViewMode m_TargetMode = ViewMode::View3D;
    double m_T = 0.0;         // 0..1
    double m_Duration = 0.4;  // seconds

    // --- Input helpers ---
    bool m_Rotating = false;
    bool m_Panning = false;
    glm::dvec2 m_LastMouse{0, 0};

    void StartTransition(ViewMode target);
    void HandleMouseInput(const editor::MouseButtonEvent&, const editor::MouseMoveEvent&, const editor::ScrollEvent&);
};

}  // namespace nc::vis
