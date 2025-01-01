#pragma once

#include <iostream>
#include <vulkan/vulkan.hpp>

#include "rendering/graphics_backend.h"

namespace v3d {
namespace rendering {
    
class VulkanBackend : public GraphicsBackend {
   public:
    VulkanBackend(Window* window) : GraphicsBackend(window) {}

    void init() {
        if (m_initialized) {
            return;
        }
        initVulkan();

        m_initialized = true;
    }

    void frame_update();

    void cleanup() { cleanup_vulkan(); }

   private:
    bool m_initialized = false;

    vk::Instance m_instance;
    vk::Device m_device;
    vk::PhysicalDevice m_physicalDevice;
    vk::DebugUtilsMessengerEXT m_debugMessenger;
#ifdef _DEBUG
	vk::DebugUtilsMessengerCreateInfoEXT m_debug_utils_create_info;
#endif

    void initVulkan();
    vk::Instance createInstance();
    vk::PhysicalDevice getSuitablePhysicalDevice();
    void cleanup_vulkan();

    void setupDebugMessenger();

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);

};
}  // namespace rendering
}  // namespace v3d