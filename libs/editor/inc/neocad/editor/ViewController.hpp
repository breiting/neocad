#pragma once

#include <memory>
#include <neocad/editor/ICamera.hpp>
#include <neocad/editor/InputEvent.hpp>

namespace nc::editor {

enum class ViewMode {
    View3D,
    Sketch2D
};

class ViewController {
   public:
    ViewController();

    void SetViewportSize(int width, int height);

    void SetCamera3D(std::shared_ptr<ICamera> cam);
    void SetCamera2D(std::shared_ptr<ICamera> cam);

    ICamera* GetActiveCamera() const;
    ViewMode GetMode() const {
        return m_Mode;
    }

    void SwitchMode(ViewMode mode);
    void OnInput(const InputEvent& ev);

    void Update(double dt);

   private:
    ViewMode m_Mode = ViewMode::View3D;
    int m_Width = 1280;
    int m_Height = 720;

    std::shared_ptr<ICamera> m_Cam3D;
    std::shared_ptr<ICamera> m_Cam2D;
};

}  // namespace nc::editor
