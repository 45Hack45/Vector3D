#pragma once

#include <GLFW/glfw3.h>
#include <plog/Log.h>

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <vulkan/vulkan.hpp>

#include "Scene.h"

namespace v3d {
const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

class Engine {
   public:
    void run() {
        m_engineStartTime = std::chrono::steady_clock::now();
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

   private:
    GLFWwindow *window;

    std::shared_ptr<Scene> m_scene;
    std::chrono::steady_clock::time_point m_engineStartTime;
    std::chrono::duration<double> m_last_frame_dt;

    void initVulkan() {}

    void mainLoop() {
        // Initialize the scene and add entities
        m_scene = Scene::create();
        auto entity = m_scene->instantiateEntity("Test object");
        auto entity2 = m_scene->instantiateEntity("Test child object", entity);
        auto entity3 = m_scene->instantiateEntity("Test grandchild object", entity2);
        auto entity4 = m_scene->instantiateEntity("Test object 2");

        m_scene->print_entities();


        // Main game loop
        while (!glfwWindowShouldClose(window)) {
            const auto frame_start = std::chrono::steady_clock::now();
            // Poll for window events
            glfwPollEvents();

            m_scene->update(m_last_frame_dt.count());

            const auto frame_end = std::chrono::steady_clock::now();
            const std::chrono::duration<double> diff = frame_end - frame_start;
            m_last_frame_dt = diff;
            std::cout << "Last frame dt (ms): " << std::chrono::duration_cast<std::chrono::milliseconds>(diff).count() << "\n";
        }
    }

    void cleanup() {
        glfwDestroyWindow(window);

        glfwTerminate();
    }
    void initWindow() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow(WIDTH, HEIGHT, "Vector3D", nullptr, nullptr);
    }
};
}  // namespace v3d
