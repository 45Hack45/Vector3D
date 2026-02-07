#pragma once

#include <algorithm>
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
    void present();

    GraphicsBackend(Window* window) : m_window(window) {
        gizmos = new GizmosManager(this);
    }
    virtual ~GraphicsBackend() { delete gizmos; }

    virtual Mesh* createMesh(std::string filePath) = 0;

    /**
     * @brief Registers a render target object to the list of render targets.
     *
     * @param renderTarget Pointer to the IRenderable object to register.
     */
    inline void registerRenderTarget(IRenderable* renderTarget) {
        m_renderTargets.push_back(renderTarget);
    }

    /**
     * @brief Unregisters a render target object from the list of rendertargets.
     * Removes the first occurrence of the given render target from the list.
     *
     * @param renderTarget Pointer to the IRenderable object to unregister.
     */
    inline void unregisterRenderTarget(IRenderable* renderTarget) {
if (!renderTarget) return;

        auto it = std::find(m_renderTargets.begin(), m_renderTargets.end(),
                            renderTarget);

        if (it != m_renderTargets.end()) m_renderTargets.erase(it);
    }

/**
     * @brief Registers a gizmos target object to the list of gizmos targets.
     *
     * @param gizmosTarget Pointer to the IGizmosRenderable object to register.
     */
    inline void registerGizmosTarget(IGizmosRenderable* gizmosTarget) {
        m_gizmosTargets.push_back(gizmosTarget);
    }

    /**
    * @brief Unregisters a gizmos target object from the list of gizmos
     * targets. Removes the first occurrence of the given gizmos target from the
     * list.
    *
* @param gizmosTarget Pointer to the IGizmosRenderable object to
     * unregister.
    */
    inline void unregisterGizmosTarget(IGizmosRenderable* gizmosTarget) {
    if (!gizmosTarget) return;

auto it = std::find(m_gizmosTargets.begin(), m_gizmosTargets.end(),
    gizmosTarget);

if (it != m_gizmosTargets.end()) m_gizmosTargets.erase(it);
    }

    GizmosManager* gizmos;
    MeshPrimitives m_primitives;  // FIXME: Make private/protected

   protected:
    Window* m_window = nullptr;
    bool m_initialized = false;

    std::vector<IRenderable*> m_renderTargets;
    std::vector<IGizmosRenderable*> m_gizmosTargets;

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
    virtual void draw_primitive_cube(glm::vec3 position, glm::vec3 scale,
                                     glm::vec4 color,
                                     bool wireframe = false) {};
    virtual void draw_primitive_sphere(glm::vec3 position, glm::vec3 scale,
                                       glm::vec4 color,
                                       bool wireframe = false) {};

   private:
    friend class GizmosManager;
};
}  // namespace rendering
}  // namespace v3d