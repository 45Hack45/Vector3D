#include "graphics_backend.h"

void v3d::rendering::GraphicsBackend::update(){
    frame_update();
    draw_gizmos();
    present_frame();
}

