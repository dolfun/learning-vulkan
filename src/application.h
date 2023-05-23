#ifndef APPLICATION_H_INCLUDED
#define APPLICATION_H_INCLUDED

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <optional>

const std::vector<const char*> requested_layers = {
    "VK_LAYER_KHRONOS_validation",
};

const std::vector<const char*> required_device_extensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};

struct QueueFamilyIndices {
    std::optional<uint32_t> graphics_family;
    std::optional<uint32_t> present_family;

    bool is_complete() {
        return graphics_family.has_value() && present_family.has_value();
    }
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> present_modes;
};

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

    // Surface
    void create_surface();
    VkSurfaceKHR surface;

    // Physical device
    void select_physical_device();
    bool is_suitable_device(VkPhysicalDevice);
    bool check_device_extension_support(VkPhysicalDevice);
    void sort_physical_devices(std::vector<VkPhysicalDevice>&);
    VkPhysicalDevice physical_device = VK_NULL_HANDLE;
    
    // Queue families
    QueueFamilyIndices find_queue_families(VkPhysicalDevice);

    // Logical device
    void create_logical_device();
    VkDevice device;

    // Queues
    VkQueue graphics_queue;
    VkQueue present_queue;

    // Swapchain queries
    SwapChainSupportDetails query_swap_chain_support(VkPhysicalDevice);
    VkSurfaceFormatKHR choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR>&);
    VkPresentModeKHR choose_swap_present_mode(const std::vector<VkPresentModeKHR>&);
    VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR&);

    // Swapchain
    void create_swap_chain();
    VkFormat swap_chain_image_format;
    VkExtent2D swap_chain_extent;
    std::vector<VkImage> swap_chain_images;
    VkSwapchainKHR swap_chain;

    // Image views
    void create_image_views();
    std::vector<VkImageView> swap_chain_image_views;
};

#endif