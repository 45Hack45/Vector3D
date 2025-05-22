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
    virtual void frame_update() {};
    virtual void cleanup() = 0;

    GraphicsBackend(Window *window) : m_window(window) {}
    virtual ~GraphicsBackend() {}

    virtual Mesh *createMesh(std::string filePath) = 0;

    /**
     * @brief Registers a render target object to the list of render targets.
     *
     * @param renderTarget Pointer to the IRenderable object to register.
     */
    void registerRenderTarget(IRenderable *renderTarget) {
        m_renderTargets.push_back(renderTarget);
    }

    // /**
    //  * @brief Unregisters a render target object from the list of render targets.
    //  *
    //  * Removes the first occurrence of the given render target from the list.
    //  *
    //  * @param renderTarget Pointer to the IRenderable object to unregister.
    //  */
    // void unregisterRenderable(IRenderable *renderable) {
    //     auto it = std::find(m_renderTargets.begin(), m_renderTargets.end(), renderable);
    //     if (it != m_renderTargets.end()) {
    //         m_renderTargets.erase(it);
    //     }
    // }

    // FIXME: Make private/protected
    MeshPrimitives m_primitives;

   protected:
    Window *m_window = nullptr;
    bool m_initialized = false;

    std::vector<IRenderable *> m_renderTargets;

    virtual void initPrimitives() = 0;

   private:
};
}  // namespace rendering
}  // namespace v3d