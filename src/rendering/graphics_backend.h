#pragma once

#include <algorithm>
#include <string>
#include <vector>

// Debug-only diagnostics
#ifndef NDEBUG
#include <plog/Log.h>

#include <boost/core/demangle.hpp>
#include <cassert>
#include <typeinfo>
#include <unordered_map>
#endif

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
#ifndef NDEBUG
        bool duplicate = std::find(m_gizmosTargets.begin(), m_gizmosTargets.end(),
                                   gizmosTarget) != m_gizmosTargets.end();
        std::string type = boost::core::demangle(typeid(*gizmosTarget).name());
        if (duplicate) {
            PLOGE << "[GIZMOS.reg] DUPLICATE registration of " << gizmosTarget
                  << " type=" << type << " count=" << m_gizmosTargets.size();
        }
        assert(!duplicate && "gizmos target registered twice");
        m_gizmosDebugInfo[gizmosTarget] = type;
        PLOGV << "[GIZMOS.reg] " << gizmosTarget << " type=" << type
              << " count=" << m_gizmosTargets.size() + 1;
#endif
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

#ifndef NDEBUG
        // MISS = unregister without a matching registration (e.g. moved object).
        if (it == m_gizmosTargets.end()) {
            PLOGW << "[GIZMOS.unreg] MISS " << gizmosTarget
                  << " not in list, count=" << m_gizmosTargets.size();
        } else {
            PLOGV << "[GIZMOS.unreg] " << gizmosTarget
                  << " type=" << m_gizmosDebugInfo[gizmosTarget]
                  << " count=" << m_gizmosTargets.size() - 1;
        }
        assert(it != m_gizmosTargets.end() &&
               "gizmos target not registered at this pointer (unregister-without-register)");
        m_gizmosDebugInfo.erase(gizmosTarget);
#endif

        if (it != m_gizmosTargets.end()) m_gizmosTargets.erase(it);
    }

    inline size_t gizmosTargetCount() const { return m_gizmosTargets.size(); }

#ifndef NDEBUG
    // Dumps every live gizmos target with its registered type.
    inline void debugDumpGizmosTargets(const char* tag) {
        PLOGV << "[GIZMOS.dump:" << tag << "] count=" << m_gizmosTargets.size();
        for (auto* t : m_gizmosTargets) {
            auto infoIt = m_gizmosDebugInfo.find(t);
            PLOGV << "[GIZMOS.dump:" << tag << "]   " << t << " type="
                  << (infoIt != m_gizmosDebugInfo.end() ? infoIt->second
                                                        : std::string("<unknown>"));
        }
    }
#endif

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
#ifndef NDEBUG
    std::unordered_map<IGizmosRenderable*, std::string> m_gizmosDebugInfo;
#endif
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