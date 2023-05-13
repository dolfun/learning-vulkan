#include "application.h"
#include "debug_messenger.h"

#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>

const uint32_t window_width = 800;
const uint32_t window_height = 600;
const std::string application_name = "hello-triangle";

const std::vector<const char*> requested_layers = {
    "VK_LAYER_KHRONOS_validation",
};

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
    if (enable_validation_layers) {
        DestroyDebugUtilsMessengerEXT(instance, debug_messenger, nullptr);
    }

    vkDestroyInstance(instance, nullptr);

    glfwDestroyWindow(window);
    glfwTerminate();
}

void Application::init_glfw() {
    if (!glfwInit()) {
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
    std::cout << "Available extensions:\n";
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
    std::cout << "Required Extensions:\n";
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

    std::cout << "Available layers:\n";
    for (const auto& validation_layer : available_layers) {
        std::cout << '\t' << validation_layer.layerName << '\n';
    }
    std::cout << "\n";

    std::cout << "Requested layers:\n";
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