#include <neocad/core/Logger.hpp>
#include <neocad/editor/ViewController.hpp>

namespace nc::editor {

ViewController::ViewController() {
}

void ViewController::SetViewportSize(int width, int height) {
    m_Width = width;
    m_Height = height;

    if (m_Cam2D) {
        m_Cam2D->SetViewport(width, height);
    }
    if (m_Cam3D) {
        m_Cam3D->SetViewport(width, height);
    }
}

void ViewController::SetCamera3D(std::shared_ptr<ICamera> cam) {
    m_Cam3D = std::move(cam);
}
void ViewController::SetCamera2D(std::shared_ptr<ICamera> cam) {
    m_Cam2D = std::move(cam);
}

ICamera* ViewController::GetActiveCamera() const {
    if (m_Mode == ViewMode::View3D)
        return m_Cam3D.get();
    return m_Cam2D.get();
}

void ViewController::SwitchMode(ViewMode mode) {
    m_Mode = mode;
}

void ViewController::OnInput(const InputEvent& ev) {
    if (ev.type == InputEventType::Key) {
        const auto& key = std::get<KeyEvent>(ev.data);
        if (key.text == '1') {
            LOG(INFO) << "Switch to 3D";
            SwitchMode(ViewMode::View3D);
        }
        if (key.text == '2') {
            SwitchMode(ViewMode::Sketch2D);
            LOG(INFO) << "Switch to 2D";
        }
        return;
    }

    auto* cam = GetActiveCamera();
    if (!cam)
        return;

    if (ev.type == InputEventType::MouseButton) {
        const auto& e = std::get<MouseButtonEvent>(ev.data);
        if (e.button == MouseButton::Left) {
            m_LMB = e.pressed;
        }
        if (e.button == MouseButton::Right) {
            m_RMB = e.pressed;
        }
        if (e.pressed) {  // FIRST PRESS
            if (auto cam = GetActiveCamera())
                cam->OnMouseStart();
        }
        return;
    }

    if (ev.type == InputEventType::MouseMove) {
        auto& mm = std::get<MouseMoveEvent>(ev.data);
        if (auto cam = GetActiveCamera()) {
            if (m_LMB)
                cam->OnMouseRotation(mm.position.x, mm.position.y);
            else if (m_RMB)
                cam->OnMousePan(mm.position.x, mm.position.y);
        }
        return;
    }

    if (ev.type == InputEventType::Scroll) {
        auto& sc = std::get<ScrollEvent>(ev.data);
        if (auto cam = GetActiveCamera())
            cam->OnMouseScroll(sc.offset.y);
        return;
    }
}

void ViewController::Update(double dt) {
    if (auto* cam = GetActiveCamera())
        cam->Update(static_cast<float>(dt));
}

}  // namespace nc::editor
