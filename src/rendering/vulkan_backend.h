#pragma once

// #ifdef __MINGW32__  // Si estamos usando MinGW
//     #define strncpy_s(dst, size, src, count) strncpy(dst, src, count)
// #endif

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include <optional>
#include <vulkan/vulkan.hpp>
// #include <iostream>

#include "rendering/graphics_backend.h"

namespace v3d {
namespace rendering {

struct VulkanDevice {
    bool initialized;
    vk::Device device;
    vk::Queue deviceQueue;
    vk::PhysicalDevice physicalDevice;
    vk::PhysicalDeviceProperties physicalDeviceProperties;
    uint32_t graphicsQueueFamilyIndex;
    vk::SurfaceKHR surface;
};

class VulkanBackend : public GraphicsBackend {
   public:
    VulkanBackend(Window* window);
    ~VulkanBackend();

    Mesh* createMesh(std::string filePath) override;

   protected:
    void initPrimitives() override {};
    void frame_update() override;
    void present_frame() override {};

   private:
    vk::Instance m_instance;
    // vk::Device m_device;
    // vk::PhysicalDevice m_physicalDevice;
    // uint32_t m_graphics_queue_index;
    VulkanDevice m_vulkanDevice;
#ifdef _DEBUG
    vk::DebugUtilsMessengerEXT m_debugMessenger;
    vk::DebugUtilsMessengerCreateInfoEXT m_debug_utils_create_info;
#endif

    vk::Instance createInstance(
        const std::optional<vk::DebugUtilsMessengerCreateInfoEXT>&
            debug_utils_create_info = std::nullopt);
    vk::SurfaceKHR createSurface(const vk::Instance& instance,
                                 GLFWwindow* window);
    vk::PhysicalDevice getSuitablePhysicalDevice(
        const vk::SurfaceKHR& surface,
        const std::vector<const char*>& requiredDeviceExtensions);
    vk::Device createLogicalDevice(
        v3d::rendering::VulkanDevice& vulkanDevice,
        const std::vector<const char*>& device_extensions,
        const vk::PhysicalDeviceFeatures& device_features);

#ifdef _DEBUG
    vk::DebugUtilsMessengerCreateInfoEXT createDebugMessenger();
    void cleanupDebugMessenger();
#endif

    static VKAPI_ATTR VkBool32 VKAPI_CALL
    debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                  VkDebugUtilsMessageTypeFlagsEXT messageType,
                  const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                  void* pUserData);
};
}  // namespace rendering
}  // namespace v3d