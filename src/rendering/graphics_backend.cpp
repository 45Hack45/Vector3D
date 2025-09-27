#include "graphics_backend.h"

void v3d::rendering::GraphicsBackend::update() {
    frame_update();
    draw_gizmos();
}
void v3d::rendering::GraphicsBackend::present() { present_frame(); }
