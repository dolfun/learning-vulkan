#ifndef APPLICATION_H_INCLUDED
#define APPLICATION_H_INCLUDED

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>

class Application {
public:
    Application();

    void run();

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    Application(Application&&) = delete;
    Application& operator=(Application&&) = delete;

    ~Application();

private:
    // Initializing glfw
    void init_glfw();
    GLFWwindow* window;

    // Initializing vulkan
    void init_vulkan();

    // Instance creation
    void create_instance();
    void check_validation_layer_support();
    VkInstance instance;

    // Debug messenger
    void setup_debug_messenger();
    VkDebugUtilsMessengerEXT debug_messenger;

    // Physical device
    void select_physical_device();
    VkPhysicalDevice physical_device = VK_NULL_HANDLE;
};

#endif