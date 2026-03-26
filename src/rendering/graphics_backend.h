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

    /// @brief Command to draw a gizmos on the next frame.
    /// @param gizmosTarget 
    inline void immediateDrawGizmos(std::unique_ptr<IGizmosRenderable> gizmosTarget) {
        m_immediateGgizmosTargets.push_back(std::move(gizmosTarget));
    }

    GizmosManager* gizmos;
    MeshPrimitives m_primitives;  // FIXME: Make private/protected

   protected:
    Window* m_window = nullptr;

    std::vector<IRenderable*> m_renderTargets;
    std::vector<IGizmosRenderable*> m_gizmosTargets;
    // Internal storage of immediate render targets
    std::vector<std::unique_ptr<IGizmosRenderable>> m_immediateGgizmosTargets;

    virtual void initPrimitives() = 0;

    virtual void frameUpdate() = 0;
    virtual void presentFrame() = 0;

    virtual void preDrawGizmosHook() {};
    virtual void postDrawGizmosHook() {};
    void drawGizmos() {
        preDrawGizmosHook();
        // Callback gizmos draw routines
        for (auto gizmosTarget : m_gizmosTargets) {
            gizmosTarget->onDrawGizmos(gizmos);
        }

        // Draw and clear immediate gizmos calls
        for (auto &gizmosTarget : m_immediateGgizmosTargets) {
            gizmosTarget->onDrawGizmos(gizmos);
        }
        m_immediateGgizmosTargets.clear();

        postDrawGizmosHook();
    };

    // Primitive draw
    virtual void drawPrimitivePoint(glm::vec3 a, float size, glm::vec4 color) {};
    virtual void drawPrimitiveLine(glm::vec3 a, glm::vec3 b, float size, glm::vec4 color) {};
    virtual void drawPrimitiveCube(glm::vec3 position, glm::vec3 scale,
                                     glm::vec4 color,
                                     bool wireframe = false) {};
    virtual void drawPrimitiveSphere(glm::vec3 position, glm::vec3 scale,
                                       glm::vec4 color,
                                       bool wireframe = false) {};

   private:
    friend class GizmosManager;
};
}  // namespace rendering
}  // namespace v3d