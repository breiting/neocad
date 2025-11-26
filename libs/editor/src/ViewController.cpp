#include <neocad/core/Logger.hpp>
#include <neocad/editor/ViewController.hpp>

namespace nc::editor {

ViewController::ViewController() {
}

void ViewController::SetViewportSize(int width, int height) {
    m_Width = width;
    m_Height = height;

    if (m_Cam2D)
        m_Cam2D->SetAspect(float(width) / float(height));
    if (m_Cam3D)
        m_Cam3D->SetAspect(float(width) / float(height));
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
    if (ev.type != InputEventType::Key)
        return;

    const auto& key = std::get<KeyEvent>(ev.data);
    if (key.text == '1') {
        LOG(INFO) << "Switch to 3D";
        SwitchMode(ViewMode::View3D);
    }
    if (key.text == '2') {
        SwitchMode(ViewMode::Sketch2D);
        LOG(INFO) << "Switch to 2D";
    }
}

}  // namespace nc::editor
