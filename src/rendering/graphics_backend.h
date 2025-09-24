#pragma once

#include <string>
#include <vector>

#include "rendering/primitives.hpp"
#include "rendering/rendering_def.h"
#include "window.h"

namespace v3d {
class Mesh;
class Engine;

namespace rendering {
class GraphicsBackend {
    friend class Engine;

   public:
    virtual void init() = 0;
    virtual void cleanup() = 0;

    void update();

    GraphicsBackend(Window *window) : m_window(window) {
        gizmos = new GizmosManager(this);
    }
    virtual ~GraphicsBackend() { delete gizmos; }

    virtual Mesh *createMesh(std::string filePath) = 0;

    /**
     * @brief Registers a render target object to the list of render targets.
     *
     * @param renderTarget Pointer to the IRenderable object to register.
     */
    inline void registerRenderTarget(IRenderable *renderTarget) {
        m_renderTargets.push_back(renderTarget);
    }

    inline void registerGizmosTarget(IGizmosRenderable *gizmosTarget) {
        m_gizmosTargets.push_back(gizmosTarget);
    }

    // /**
    //  * @brief Unregisters a render target object from the list of render
    //  targets.
    //  *
    //  * Removes the first occurrence of the given render target from the list.
    //  *
    //  * @param renderTarget Pointer to the IRenderable object to unregister.
    //  */
    // void unregisterRenderable(IRenderable *renderable) {
    //     auto it = std::find(m_renderTargets.begin(), m_renderTargets.end(),
    //     renderable); if (it != m_renderTargets.end()) {
    //         m_renderTargets.erase(it);
    //     }
    // }

    GizmosManager *gizmos;
    MeshPrimitives m_primitives;  // FIXME: Make private/protected

   protected:
    Window *m_window = nullptr;
    bool m_initialized = false;

    std::vector<IRenderable *> m_renderTargets;
    std::vector<IGizmosRenderable *> m_gizmosTargets;

    virtual void initPrimitives() = 0;

    virtual void frame_update() = 0;
    virtual void present_frame() = 0;

    virtual void pre_draw_gizmos_hook() {};
    virtual void post_draw_gizmos_hook() {};
    void draw_gizmos() {
        pre_draw_gizmos_hook();
        for (auto renderTarget : m_gizmosTargets) {
            renderTarget->onDrawGizmos(gizmos);
        }
        post_draw_gizmos_hook();
    };

    // Inmediate primitive draw
    virtual void draw_primitive_point(glm::vec3 a, float size) {};
    virtual void draw_primitive_line(glm::vec3 a, glm::vec3 b, float size) {};
    virtual void draw_primitive_cube(glm::vec3 position, float size,
                                     glm::vec4 color) {};
    virtual void draw_primitive_sphere(glm::vec3 position, float size,
                                       glm::vec4 color) {};

   private:
    friend class GizmosManager;
};
}  // namespace rendering
}  // namespace v3d