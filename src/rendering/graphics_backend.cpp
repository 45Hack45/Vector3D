#include "graphics_backend.h"

void v3d::rendering::GraphicsBackend::update() {
    frameUpdate();
    drawGizmos();
}
void v3d::rendering::GraphicsBackend::present() { presentFrame(); }
