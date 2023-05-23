#include "application.h"
#include "debug_messenger.h"
#include "utility.h"

#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <set>

const uint32_t window_width = 800;
const uint32_t window_height = 600;
const std::string application_name = "hello-triangle";

#ifdef NDEBUG
    const bool enable_validation_layers = false;
#else
    const bool enable_validation_layers = true;
#endif

Application::Application() {
    init_glfw();
    init_vulkan();
}

void Application::run() {
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }
}

Application::~Application() {
    for (auto image_view : swap_chain_image_views) {
        vkDestroyImageView(device, image_view, nullptr);
    }

    vkDestroySwapchainKHR(device, swap_chain, nullptr);
    vkDestroyDevice(device, nullptr);

    if (enable_validation_layers) {
        DestroyDebugUtilsMessengerEXT(instance, debug_messenger, nullptr);
    }

    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);

    glfwDestroyWindow(window);

    glfwTerminate();
}

void Application::init_glfw() {
    if (glfwInit() != GLFW_TRUE) {
        throw std::runtime_error("Failed to initialize glfw.");
    }
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(window_width, window_height, application_name.c_str(), nullptr, nullptr);
    if (window == nullptr) {
        throw std::runtime_error("Failed to create a window.");
    }
}

void Application::init_vulkan() {
    create_instance();
    setup_debug_messenger();
    create_surface();
    select_physical_device();
    create_logical_device();
    create_swap_chain();
    create_image_views();
}

void Application::create_instance() {
    // Application Info
    VkApplicationInfo app_info{};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = application_name.c_str();
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "no-engine";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_0;

    // Instance Create Info
    VkInstanceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;

    // Validation Layers
    // And debugging instance creation and destruction
    VkDebugUtilsMessengerCreateInfoEXT debug_create_info{};
    if (enable_validation_layers) {
        check_validation_layer_support();
        create_info.enabledLayerCount = static_cast<uint32_t>(requested_layers.size());
        create_info.ppEnabledLayerNames = requested_layers.data();

        populate_debug_messenger_create_info(debug_create_info);
        create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debug_create_info;
    } else {
        create_info.enabledLayerCount = 0;
        create_info.pNext = nullptr;
    }

    // Checking available extensions
    uint32_t available_extension_count = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &available_extension_count, nullptr);
    std::vector<VkExtensionProperties> available_extensions(available_extension_count);
    vkEnumerateInstanceExtensionProperties(nullptr, &available_extension_count, available_extensions.data());
    std::cout << "Available extensions(" << available_extension_count << "):\n";
    for (const auto& extension : available_extensions) {
        std::cout << '\t' << extension.extensionName << '\n';
    }
    std::cout << '\n';

    // Required extensions
    uint32_t glfw_extension_count = 0;
    const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
    std::vector<const char*> required_extensions(glfw_extensions, glfw_extensions + glfw_extension_count);
    if (enable_validation_layers) {
        required_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
    std::cout << "Required Extensions(" << required_extensions.size() << "):\n";
    for (const auto& extension_name : required_extensions) {
        std::cout << '\t' << extension_name << '\n';
    }
    std::cout << "\n\n";

    // Checking availability of required extensions
    for (const auto& required_extension : required_extensions) {
        std::string required_extension_str = required_extension;
        bool extension_found = false;
        for (const auto& extension : available_extensions) {
            if (required_extension_str == extension.extensionName) {
                extension_found = true;
                break;
            }
        }
        if (!extension_found) {
            throw std::runtime_error("Cannot find extension named " + required_extension_str + " required by glfw.");
        }
    }

    // Adding extensions to create_info
    create_info.enabledExtensionCount = static_cast<uint32_t>(required_extensions.size());
    create_info.ppEnabledExtensionNames = required_extensions.data();

    // Creating instance
    VkResult result = vkCreateInstance(&create_info, nullptr, &instance);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create instance.");
    }
}

void Application::check_validation_layer_support() {
    uint32_t layer_count;
    vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
    std::vector<VkLayerProperties> available_layers(layer_count);
    vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

    std::cout << "Available layers(" << layer_count << "):\n";
    for (const auto& validation_layer : available_layers) {
        std::cout << '\t' << validation_layer.layerName << '\n';
    }
    std::cout << "\n";

    std::cout << "Requested layers(" << requested_layers.size() << "):\n";
    for (const auto& validation_layer : requested_layers) {
        std::cout << '\t' << validation_layer << '\n';
    }
    std::cout << "\n\n";

    for (const auto& required_validation_layer : requested_layers) {
        std::string required_validation_layer_name = required_validation_layer;
        bool layer_found = false;
        for (const auto& available_validation_layer : available_layers) {
            if (required_validation_layer_name == available_validation_layer.layerName) {
                layer_found = true;
                break;
            }
        }
        if (!layer_found) {
            throw std::runtime_error("Cannot find layer named " + required_validation_layer_name + ".");
        }
    }
}

void Application::setup_debug_messenger() {
    if (!enable_validation_layers) return;

    VkDebugUtilsMessengerCreateInfoEXT create_info{};
    populate_debug_messenger_create_info(create_info);
    

    if (CreateDebugUtilsMessengerEXT(instance, &create_info, nullptr, &debug_messenger) != VK_SUCCESS) {
        throw std::runtime_error("Failed to set up debug messenger.");
    }
}

void Application::create_surface() {
    if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create window surface.");
    }
}

void Application::select_physical_device() {
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(instance, &device_count, nullptr);
    if (device_count == 0) {
        throw std::runtime_error("Failed to find GPUs with Vulkan support.");
    }
    std::vector<VkPhysicalDevice> devices(device_count);
    vkEnumeratePhysicalDevices(instance, &device_count, devices.data());

    sort_physical_devices(devices);

    std::cout << "Found " << device_count << " GPU(s) with vulkan support:\n";
    for (const auto& device : devices) {
        print_physical_device_info(device, (device == devices.front()));
    }

    physical_device = devices.front();
}

QueueFamilyIndices Application::find_queue_families(VkPhysicalDevice device) {
    QueueFamilyIndices indices;

    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);
    std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());

    int i = 0;
    for (const auto& queue_family : queue_families) {
        if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphics_family = i;
        }

        VkBool32 present_support = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &present_support);
        if (present_support) {
            indices.present_family = i;
        }

        if (indices.is_complete()) break;
        ++i;
    }

    return indices;
}

void Application::create_logical_device() {
    // Queue families
    QueueFamilyIndices indices = find_queue_families(physical_device);

    // Device queue create info
    std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
    std::set<uint32_t> unique_queue_families = {
        indices.graphics_family.value(),
        indices.present_family.value(),
    };

    float queue_priority = 1.0f;
    for (auto queue_family : unique_queue_families) {
        VkDeviceQueueCreateInfo queue_create_info{};
        queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_info.queueFamilyIndex = queue_family;
        queue_create_info.queueCount = 1;
        queue_create_info.pQueuePriorities = &queue_priority;
        queue_create_infos.push_back(queue_create_info);

    }

    // Physical device features
    VkPhysicalDeviceFeatures device_features{};

    // Device create info
    VkDeviceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
    create_info.pQueueCreateInfos = queue_create_infos.data();

    create_info.pEnabledFeatures = &device_features;

    // Device extensions
    std::cout << "Required device extensions(" << required_device_extensions.size() << "):\n";
    for (const auto& extension : required_device_extensions) {
        std::cout << "\t" << extension << '\n';
    }
    std::cout << '\n';
    
    create_info.enabledExtensionCount = static_cast<uint32_t>(required_device_extensions.size());
    create_info.ppEnabledExtensionNames = required_device_extensions.data();

    // Device layers
    // Not necessary
    if (enable_validation_layers) {
        create_info.enabledLayerCount = static_cast<uint32_t>(requested_layers.size());
        create_info.ppEnabledLayerNames = requested_layers.data();
    } else {
        create_info.enabledLayerCount = 0;
    }

    // Device creation
    if (vkCreateDevice(physical_device, &create_info, nullptr, &device) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create logical device.");
    }

    // Queue handle
    vkGetDeviceQueue(device, indices.graphics_family.value(), 0, &graphics_queue);
    vkGetDeviceQueue(device, indices.present_family.value(), 0, &present_queue);
}

void Application::create_swap_chain() {
    auto swap_chain_support = query_swap_chain_support(physical_device);
    auto surface_format = choose_swap_surface_format(swap_chain_support.formats);
    auto present_mode = choose_swap_present_mode(swap_chain_support.present_modes);
    auto extent = choose_swap_extent(swap_chain_support.capabilities);

    uint32_t image_count = swap_chain_support.capabilities.minImageCount + 1;
    if (swap_chain_support.capabilities.maxImageCount > 0) {
        image_count = std::min(image_count, swap_chain_support.capabilities.maxImageCount);
    }

    VkSwapchainCreateInfoKHR create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.surface = surface;

    create_info.minImageCount = image_count;
    create_info.imageFormat = surface_format.format;
    create_info.imageColorSpace = surface_format.colorSpace;
    create_info.imageExtent = extent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = find_queue_families(physical_device);
    uint32_t queue_family_indices[] = {indices.graphics_family.value(), indices.present_family.value()};

    if (indices.graphics_family != indices.present_family) {
        create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        create_info.queueFamilyIndexCount = 2;
        create_info.pQueueFamilyIndices = queue_family_indices;
    } else {
        create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

        // Optional
        create_info.queueFamilyIndexCount = 0;
        create_info.pQueueFamilyIndices = nullptr;
    }

    create_info.preTransform = swap_chain_support.capabilities.currentTransform;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    create_info.presentMode = present_mode;
    create_info.clipped = VK_TRUE;
    create_info.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(device, &create_info, nullptr, &swap_chain) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create swap chain.");
    }

    vkGetSwapchainImagesKHR(device, swap_chain, &image_count, nullptr);
    swap_chain_images.resize(image_count);
    vkGetSwapchainImagesKHR(device, swap_chain, &image_count, swap_chain_images.data());

    swap_chain_image_format = surface_format.format;
    swap_chain_extent = extent;
}

void Application::create_image_views() {
    swap_chain_image_views.resize(swap_chain_images.size());

    for (size_t i = 0; i < swap_chain_images.size(); ++i) {
        VkImageViewCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        create_info.image = swap_chain_images[i];

        create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        create_info.format = swap_chain_image_format;

        create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        create_info.subresourceRange.baseMipLevel = 0;
        create_info.subresourceRange.levelCount = 1;
        create_info.subresourceRange.baseArrayLayer = 0;
        create_info.subresourceRange.layerCount = 1;

        if (vkCreateImageView(device, &create_info, nullptr, &swap_chain_image_views[i]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create image views.");
        }
    }
}