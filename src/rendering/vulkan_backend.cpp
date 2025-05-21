#include "rendering/vulkan_backend.h"
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

#include <plog/Log.h>
#include "utils/exception.hpp"

#include <cstring>
#include <map>
#include "utils/utils.hpp"

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"};

const std::vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

namespace v3d {
namespace rendering {

Mesh* VulkanBackend::createMesh(std::string filePath) {
    throw exception::NotImplemented();
}

bool validate_extensions(const std::vector<const char*>& required,
                         const std::vector<vk::ExtensionProperties>& available) {
    // inner find_if gives true if the extension was not found
    // outer find_if gives true if none of the extensions were not found, that is if all extensions were found
    return std::find_if(required.begin(),
                        required.end(),
                        [&available](auto extension) {
                            return std::find_if(available.begin(),
                                                available.end(),
                                                [&extension](auto const& ep) {
                                                    return strcmp(ep.extensionName, extension) == 0;
                                                }) == available.end();
                        }) == required.end();
}

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

#ifdef _DEBUG
vk::DebugUtilsMessengerCreateInfoEXT VulkanBackend::createDebugMessenger() {
    assert(enableValidationLayers && "Tried to create debug messenger when validation layers are disabled!");

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
    return vk::DebugUtilsMessengerCreateInfoEXT(
        vk::DebugUtilsMessengerCreateFlagsEXT(), severity_flags, msg_type_flags,
        debugCallback, nullptr);
}

void VulkanBackend::cleanupDebugMessenger() {
    if (m_debugMessenger) {
        m_instance.destroyDebugUtilsMessengerEXT(m_debugMessenger);
    } else {
        PLOGW << "Tried to cleanup debug messenger that was not initialized" << std::endl;
    }
}
#endif

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanBackend::debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {
    std::stringstream ss;
    ss << pCallbackData->messageIdNumber << " - " << pCallbackData->pMessageIdName << ": " << pCallbackData->pMessage;
    std::string msg = ss.str();

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

std::optional<uint32_t> getGraphicsQueueFamilyIndex(const vk::PhysicalDevice& physicalDevice, const vk::SurfaceKHR& surface) {
    std::vector<vk::QueueFamilyProperties> queueFamilyProperties = physicalDevice.getQueueFamilyProperties();
    std::optional<uint32_t> queueFamilyIndex = -1;

    for (uint32_t index = 0; index < utils::to_u32(queueFamilyProperties.size()); index++) {
        const auto& queueFamily = queueFamilyProperties[index];
        vk::Bool32 supportsPresent = physicalDevice.getSurfaceSupportKHR(index, surface);

        // Find a queue family which supports graphics and presentation.
        if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics && supportsPresent) {
            queueFamilyIndex = index;
            break;
        }
    }
    return queueFamilyIndex;
}

int rateDeviceSuitability(const vk::PhysicalDevice& physicalDevice, const vk::SurfaceKHR& surface, const std::vector<const char*>& requiredDeviceExtensions) {
    vk::PhysicalDeviceProperties deviceProperties = physicalDevice.getProperties();
    vk::PhysicalDeviceFeatures deviceFeatures = physicalDevice.getFeatures();

    std::vector<vk::QueueFamilyProperties> queueFamilyProperties = physicalDevice.getQueueFamilyProperties();

    if (queueFamilyProperties.size() == 0) {
        // No queue families
        return -1000;
    }

    int score = 0;

    if (deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
        score += 1000;
    }

    std::optional<uint32_t> queueFamilyIndex = getGraphicsQueueFamilyIndex(physicalDevice, surface);
    if (!queueFamilyIndex.has_value()) {
        // The device doesn't support queue families with presentation support
        return -1000;
    }

    std::vector<vk::ExtensionProperties> deviceExtensions = physicalDevice.enumerateDeviceExtensionProperties();
	if (!validate_extensions(requiredDeviceExtensions, deviceExtensions))
	{
		// Required device extensions are missing.
        return -1000;
	}

    // Maximum possible size of textures affects graphics quality
    score += deviceProperties.limits.maxImageDimension2D;

    return score;
}

vk::PhysicalDevice VulkanBackend::getSuitablePhysicalDevice(const vk::SurfaceKHR& surface, const std::vector<const char*>& requiredDeviceExtensions) {
    std::vector<vk::PhysicalDevice> gpus = m_instance.enumeratePhysicalDevices();
    vk::PhysicalDevice physicalDevice;

    if (gpus.size() == 0) {
        throw std::runtime_error("Failed to find a GPU with Vulkan support!");
    }

    std::multimap<int, vk::PhysicalDevice> candidates;

    for (const auto& gpu : gpus) {
        vk::PhysicalDeviceProperties deviceProperties = gpu.getProperties();
        int score = rateDeviceSuitability(gpu, surface, requiredDeviceExtensions);
        PLOGD << "Evaluating physical device '" << (char*)deviceProperties.deviceName << "' with a score of " << score << std::endl;
        candidates.insert(std::make_pair(score, gpu));
    }

    if (candidates.rbegin()->first > 0) {
        physicalDevice = candidates.rbegin()->second;
        int score = candidates.rbegin()->first;
        vk::PhysicalDeviceProperties deviceProperties = physicalDevice.getProperties();
        PLOGI << "Selected the physical device '" << (char*)deviceProperties.deviceName << "' with a score of " << score << std::endl;
    } else {
        throw std::runtime_error("Failed to find a suitable GPU!");
    }

    return physicalDevice;
}

vk::Instance VulkanBackend::createInstance(const std::optional<vk::DebugUtilsMessengerCreateInfoEXT>& debug_utils_create_info) {
    PLOGD << "Creating Vulkan instance" << std::endl;

    // Initialize vulkan dynamic loader
    static vk::DynamicLoader dl;
    PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
    VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

#ifdef _DEBUG
    if (enableValidationLayers && !checkValidationLayerSupport()) {
        throw std::runtime_error(
            "Validation layers requested, but not available!");
    }
    if (enableValidationLayers && !debug_utils_create_info.has_value()) {
        throw std::runtime_error(
            "Validation layers enabled, but not DebugUtilsMessengerCreateInfoEXT configured!");
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

#ifdef _DEBUG
    // Set instance creation validation layer
    instanceCreateInfo.pNext = &debug_utils_create_info;
#endif

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

vk::SurfaceKHR VulkanBackend::createSurface(const vk::Instance& instance, GLFWwindow* window) {
    VkSurfaceKHR surface;
    if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
    return vk::SurfaceKHR(surface);
}

vk::Device v3d::rendering::VulkanBackend::createLogicalDevice(v3d::rendering::VulkanDevice& vulkanDevice, const std::vector<const char*>& device_extensions, const vk::PhysicalDeviceFeatures& device_features) {
    // Create a device with one queue
    float queue_priority = 1.0f;
    vk::DeviceQueueCreateInfo queue_info({}, vulkanDevice.graphicsQueueFamilyIndex, 1, &queue_priority);
    vk::DeviceCreateInfo device_info({}, queue_info, {}, device_extensions, &device_features);

    try {
        vk::Device device = vulkanDevice.physicalDevice.createDevice(device_info);
        PLOGD << "Vulkan device created" << std::endl;

        // initialize function pointers for device
        VULKAN_HPP_DEFAULT_DISPATCHER.init(device);
        return device;

    } catch (std::exception const& e) {
        PLOGE << e.what() << std::endl;
        throw std::runtime_error("Failed to create logical device!");
    }
}

void VulkanBackend::initVulkan() {
    PLOGI << "Initializing Vulkan" << std::endl;

#ifdef _DEBUG
    m_debug_utils_create_info = createDebugMessenger();
    m_instance = createInstance(m_debug_utils_create_info);
    m_debugMessenger = m_instance.createDebugUtilsMessengerEXT(m_debug_utils_create_info);
#else
    m_instance = createInstance();
#endif

    m_vulkanDevice.surface = createSurface(m_instance, m_window->getWindow());

    m_vulkanDevice.physicalDevice = getSuitablePhysicalDevice(m_vulkanDevice.surface, deviceExtensions);
    m_vulkanDevice.graphicsQueueFamilyIndex = getGraphicsQueueFamilyIndex(m_vulkanDevice.physicalDevice, m_vulkanDevice.surface).value();
    m_vulkanDevice.device = createLogicalDevice(m_vulkanDevice, deviceExtensions, {});
    m_vulkanDevice.deviceQueue = m_vulkanDevice.device.getQueue(m_vulkanDevice.graphicsQueueFamilyIndex, 0);

    m_vulkanDevice.initialized = true;

    PLOGI << "Vulkan initialized" << std::endl;
}

void VulkanBackend::frame_update() {}

void VulkanBackend::cleanup_vulkan() {
    if (!m_initialized) {
        return;
    }

    if (m_vulkanDevice.initialized) {
        m_vulkanDevice.device.destroy();
    }

#ifdef _DEBUG
    cleanupDebugMessenger();
#endif

    if (m_vulkanDevice.initialized) m_instance.destroySurfaceKHR(m_vulkanDevice.surface);
    m_instance.destroy();

    m_initialized = false;
}
}  // namespace rendering

}  // namespace v3d
