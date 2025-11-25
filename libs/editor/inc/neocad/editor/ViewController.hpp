#pragma once
#include <glm/glm.hpp>
#include <neocad/editor/ICamera.hpp>
#include <neocad/editor/InputEvent.hpp>

namespace nc::editor {

enum class ViewMode {
    View3D,
    Sketch2D
};

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

    void SetCameras(std::shared_ptr<ICamera> cam2d, std::shared_ptr<ICamera> cam3d);

   private:
    std::shared_ptr<ICamera> m_Cam2D;
    std::shared_ptr<ICamera> m_Cam3D;

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

}  // namespace nc::editor
