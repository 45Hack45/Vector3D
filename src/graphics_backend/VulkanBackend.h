#pragma once

#include <iostream>
#include <vulkan/vulkan.hpp>

#include "GraphicsBackend.h"

namespace v3d {
class VulkanBackend : public GraphicsBackend {
   public:
    VulkanBackend(GLFWwindow* window) : GraphicsBackend(window) {}

    void init() {
        if (initialized) {
            return;
        }
        initVulkan();

        initialized = true;
    }

    void frame_update();

    void destroy() { cleanup(); }

   private:
    bool initialized = false;

    vk::Instance instance{nullptr};
    vk::Device device{nullptr};
    vk::DebugUtilsMessengerEXT debugMessenger;
#ifdef _DEBUG
	vk::DebugUtilsMessengerCreateInfoEXT debug_utils_create_info;
#endif

    void initVulkan();
    void createInstance();
    std::vector<const char*> getRequiredExtensions();

    bool checkValidationLayerSupport();

    void setupDebugMessenger();

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);

    void cleanup();
};
}  // namespace v3d