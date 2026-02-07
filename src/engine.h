#pragma once

#include <glad/glad.h>

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include "ModelManager.hpp"
#include "editor/ComponentRegistry.h"
#include "editor/Editor.h"
#include "input/InputDevice.hpp"
#include "input/InputKeys.hpp"
#include "input/InputManager.h"
#include "physics/physics.h"
#include "plog/Log.h"
#include "rendering/null_graphics_backend.hpp"
#include "rendering/opengl_backend.h"
#include "rendering/rendering_def.h"
#include "rendering/vulkan_backend.h"
#include "scene.h"
#include "window.h"

namespace v3d {
class Engine {
   public:
    Engine() : m_window(new Window()) {};
    Engine(uint32_t width, uint32_t height)
        : m_window(new Window(width, height)) {};
    Engine(uint32_t width, uint32_t height,
           rendering::GraphicsBackendType graphicsBackendType)
        : m_window(new Window(width, height)),
          m_gBackendType(graphicsBackendType) {};
    virtual ~Engine() {}

    void run() {
        m_engineStartTime = std::chrono::steady_clock::now();
        init();
        start();
        mainLoop();
        cleanup();
    }

    inline void registerRenderTarget(rendering::IRenderable* renderTarget) {
        m_graphicsBackend->registerRenderTarget(renderTarget);
    }
    inline void unregisterRenderTarget(rendering::IRenderable* renderTarget) {
        m_graphicsBackend->unregisterRenderTarget(renderTarget);
    }

    inline void registerGizmosTarget(
        rendering::IGizmosRenderable* gizmosTarget) {
        m_graphicsBackend->registerGizmosTarget(gizmosTarget);
    }
    inline void unregisterGizmosTarget(
        rendering::IGizmosRenderable* gizmosTarget) {
        m_graphicsBackend->unregisterGizmosTarget(gizmosTarget);
    }

    InputManager* getInputManager() { return &m_inputManager; }

   protected:
    // TODO: change member pointers to smart pointers
    editor::Editor* m_editor;
    std::unique_ptr<ModelManager> m_modelManager = nullptr;

    rendering::GraphicsBackendType m_gBackendType =
        rendering::GraphicsBackendType::VULKAN_API;
    rendering::GraphicsBackend* m_graphicsBackend;
    rendering::VulkanBackend* m_vulkanBackend;
    rendering::OpenGlBackend* m_openGlBackend;
    rendering::NullGraphicsBackend* m_nullGraphicsBackend;
    Window* m_window;

    bool m_initialized = false;
    int m_targetFrameRate = 60;

    InputManager m_inputManager;

    void initDefaultInput();

    Physics m_phSystem;
    std::shared_ptr<Scene> m_scene;
    std::chrono::steady_clock::time_point m_engineStartTime;
    std::chrono::duration<double> m_last_frame_dt =
        std::chrono::duration<double>(1 / 60);

    /// @brief Called after engine initialization, but before all components
    /// have been initialized
    virtual void engineStartPre() {}
    /// @brief Called after all components have been initialized and before the
    /// main loop starts
    virtual void engineStart() {}

    // TODO: Implement
    virtual void logicFrameUpdate() {}
    virtual void physicsFrameUpdate() {}
    virtual void graphicsFrameUpdate() {}
    virtual void editorGUIFrameUpdate() {}

    virtual void cleanupHook() {}

    virtual std::unique_ptr<ModelLoader> makeModelLoader() {
        // Init Model manager with Assimp as loader
        Assimp::DefaultLogger::create("AssimpLog.txt", Assimp::Logger::VERBOSE);
        AssimpLoader::attachDefaultLogger();

        AssimpLoaderPropertes properties;
        return std::make_unique<AssimpLoader>(std::move(properties));
    }

   private:
    editor::EditorComponentRegistry* m_componentRegistry;

    void init();
    void start();
    void mainLoop();
    void cleanup();

    void processInput(GLFWwindow* window);
    /// @brief Pre-initialize scene component vectors, required to be able to
    /// add components dynamically
    /// @param scene
    /// @param componentRegistry
    void registerComponents(Scene* scene,
                            editor::EditorComponentRegistry* componentRegistry);
};
}  // namespace v3d
