#include "rendering/vulkan_backend.h"
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

#include <plog/Log.h>

#include <cstring>

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

namespace v3d {
namespace rendering {

std::vector<const char*> getRequiredExtensions() {
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

bool checkValidationLayerSupport() {
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

int rateDeviceSuitability(const vk::PhysicalDevice device) {
    vk::PhysicalDeviceProperties deviceProperties = device.getProperties();
    vk::PhysicalDeviceFeatures deviceFeatures = device.getFeatures();

    std::vector<vk::QueueFamilyProperties> queueFamilyProperties =
        device.getQueueFamilyProperties();

    int score = 0;

    if (deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
        score += 1000;
    }

    // Maximum possible size of textures affects graphics quality
    score += deviceProperties.limits.maxImageDimension2D;

    return score;
}

void VulkanBackend::setupDebugMessenger() {
    if (!enableValidationLayers) return;

    // Setup debug messenger
    vk::DebugUtilsMessageSeverityFlagsEXT severity_flags =
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;

    vk::DebugUtilsMessageTypeFlagsEXT msg_type_flags =
        vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
        vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
        vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;

    // This config will be used for the instance creation and
    // afterwards for all vulkan validation layers debug messages
    m_debug_utils_create_info = vk::DebugUtilsMessengerCreateInfoEXT(
        vk::DebugUtilsMessengerCreateFlagsEXT(), severity_flags, msg_type_flags,
        debugCallback, nullptr);
}

void VulkanBackend::cleanupDebugMessenger(){
    if (m_debugMessenger) {
        m_instance.destroyDebugUtilsMessengerEXT(m_debugMessenger);
    }else{
        PLOGW << "Tried to cleanup debug messenger that was not initialized" << std::endl;
    }
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

vk::PhysicalDevice VulkanBackend::getSuitablePhysicalDevice() {
    std::vector<vk::PhysicalDevice> gpus =
        m_instance.enumeratePhysicalDevices();
    vk::PhysicalDevice physicalDevice;

    if (gpus.size() == 0) {
        throw std::runtime_error("Failed to find a GPU with Vulkan support!");
    }

    std::multimap<int, vk::PhysicalDevice> candidates;

    for (const auto& gpu : gpus) {
        int score = rateDeviceSuitability(gpu);
        candidates.insert(std::make_pair(score, gpu));
    }

    if (candidates.rbegin()->first > 0) {
        physicalDevice = candidates.rbegin()->second;
    } else {
        throw std::runtime_error("Failed to find a suitable GPU!");
    }

    return physicalDevice;
}

void VulkanBackend::initVulkan() {
    PLOGI << "Initializing Vulkan" << std::endl;
    m_instance = createInstance();
    m_debugMessenger = m_instance.createDebugUtilsMessengerEXT(m_debug_utils_create_info);
    m_physicalDevice = getSuitablePhysicalDevice();
    
    PLOGI << "Vulkan initialized" << std::endl;
}

vk::Instance VulkanBackend::createInstance() {
    PLOGD << "Creating Vulkan instance" << std::endl;

    // Initialize vulkan dynamic loader
    static vk::DynamicLoader dl;
    PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
	VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

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

    // Print extensions
    IF_PLOG(plog::verbose) {
        std::string extensionsStr;
        for (const char* extension : extensions) {
            extensionsStr += std::string(extension) + "; ";
        }
        PLOGV << "Requested extensions: " << extensionsStr << std::endl;
    }

    vk::InstanceCreateInfo instanceCreateInfo(
        vk::InstanceCreateFlags(), &applicationInfo,
        enableValidationLayers ? static_cast<uint32_t>(validationLayers.size())
                               : 0,
        enableValidationLayers ? validationLayers.data() : nullptr,
        static_cast<uint32_t>(extensions.size()), extensions.data());

if (enableValidationLayers){
    setupDebugMessenger();
    instanceCreateInfo.pNext = &m_debug_utils_create_info;
}

    try {
        vk::Instance instance = vk::createInstance(instanceCreateInfo, nullptr);
VULKAN_HPP_DEFAULT_DISPATCHER.init(instance);
        PLOGD << "Vulkan instance created" << std::endl;
                return instance;
    } catch (std::exception const& e) {
        PLOGE << e.what() << std::endl;
        throw std::runtime_error("Failed to create Vulkan instance!");
    }
}

void VulkanBackend::frame_update() {}

void VulkanBackend::cleanup_vulkan() {
    if (!m_initialized) {
        return;
    }

    if (enableValidationLayers) {
        cleanupDebugMessenger();
    }
    m_instance.destroy();

    m_initialized = false;
}
}  // namespace rendering

}  // namespace v3d
