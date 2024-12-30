#include "graphics_backend/VulkanBackend.h"

#include <plog/Log.h>

#include <cstring>

#include "VulkanBackend.h"

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

namespace v3d {
void VulkanBackend::frame_update() {}
void VulkanBackend::initVulkan() { createInstance(); }

void VulkanBackend::createInstance() {
    PLOGD << "Creating Vulkan instance" << std::endl;

#ifdef _DEBUG
    if (enableValidationLayers && !checkValidationLayerSupport()) {
        throw std::runtime_error(
            "validation layers requested, but not available!");
    }
#endif

    // Application info
    vk::ApplicationInfo applicationInfo("Vulkan Example", 1, "No Engine", 1,
                                        VK_API_VERSION_1_0);

    std::vector<const char*> extensions = getRequiredExtensions();

    IF_PLOG(plog::debug) {
        std::string extensionsStr;
        for (const char* extension : extensions) {
            extensionsStr += std::string(extension) + "; ";
        }
        PLOGD << "Requested extensions: " << extensionsStr << std::endl;
    }

    vk::InstanceCreateInfo instanceCreateInfo(
        vk::InstanceCreateFlags(), &applicationInfo,
        enableValidationLayers ? static_cast<uint32_t>(validationLayers.size())
                               : 0,
        enableValidationLayers ? validationLayers.data() : nullptr,
        static_cast<uint32_t>(extensions.size()), extensions.data());

#ifdef _DEBUG
    setupDebugMessenger();
    instanceCreateInfo.pNext = &debug_utils_create_info;
#endif

    try {
        instance = vk::createInstance(instanceCreateInfo, nullptr);
        // VULKAN_HPP_DEFAULT_DISPATCHER.init(instance);
    } catch (vk::SystemError error) {
        throw std::runtime_error(
            std::string("Failed to create Vulkan instance: ") + error.what());
    }
    PLOGD << "Vulkan instance created" << std::endl;
}

void VulkanBackend::cleanup() {
    if (!initialized) {
        return;
    }

    instance.destroy();
    instance = nullptr;

    initialized = false;
}

std::vector<const char*> VulkanBackend::getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions,
                                        glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers) {
        // Extensions to set a callback to handle the validation layer messages
        extensions.push_back(vk::EXTDebugUtilsExtensionName);
    }

    return extensions;
}

bool VulkanBackend::checkValidationLayerSupport() {
    std::vector<vk::LayerProperties> availableLayers =
        vk::enumerateInstanceLayerProperties();

    for (const char* layerName : validationLayers) {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }
    return true;
}

void VulkanBackend::setupDebugMessenger() {
    if (!enableValidationLayers) return;

    vk::DebugUtilsMessageSeverityFlagsEXT severity_flags =
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;

    vk::DebugUtilsMessageTypeFlagsEXT msg_type_flags =
        vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
        vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
        vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;

    debug_utils_create_info = vk::DebugUtilsMessengerCreateInfoEXT(
        vk::DebugUtilsMessengerCreateFlagsEXT(), severity_flags, msg_type_flags,
        debugCallback, nullptr);
}

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanBackend::debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {
    std::string msg =
        std::format("{} - {}: {}", pCallbackData->messageIdNumber,
                    pCallbackData->pMessageIdName, pCallbackData->pMessage);

    if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        PLOGE << msg << std::endl;
    } else if (messageSeverity &
               VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        PLOGW << msg << std::endl;
    } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
        PLOGI << msg << std::endl;
    } else if (messageSeverity &
               VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
        PLOGV << msg << std::endl;
    }
    return VK_FALSE;
}

}  // namespace v3d
