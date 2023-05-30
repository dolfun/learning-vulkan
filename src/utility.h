#ifndef UTILITY_H_INCLUDED
#define UTILITY_H_INCLUDED

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <limits>

#include "application.h"

#define PREFERRED_DEVICE_TYPE VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU

struct Vertex {
    glm::vec2 pos;
    glm::vec3 color;

    static auto get_binding_description() {
        VkVertexInputBindingDescription binding_description{};
        binding_description.binding = 0;
        binding_description.stride = sizeof(Vertex);
        binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return binding_description;
    }

    static auto get_attribute_description() {
        std::array<VkVertexInputAttributeDescription, 2> attribute_descriptions;

        attribute_descriptions[0].binding = 0;
        attribute_descriptions[0].location = 0;
        attribute_descriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attribute_descriptions[0].offset = offsetof(Vertex, pos);

        attribute_descriptions[1].binding = 0;
        attribute_descriptions[1].location = 1;
        attribute_descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attribute_descriptions[1].offset = offsetof(Vertex, color);

        return attribute_descriptions;
    }
};

template<typename T>
size_t get_vector_data_size(const std::vector<T>& vec) {
    return vec.size() * sizeof(T);
}

uint64_t get_device_dedicated_vram_size(const VkPhysicalDevice& device) {
    VkPhysicalDeviceMemoryProperties memory_properties;
    vkGetPhysicalDeviceMemoryProperties(device, &memory_properties);

    uint32_t memory_type_index = 0;
    for (uint32_t i = 0; i < memory_properties.memoryTypeCount; ++i) {
        if ((memory_properties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) != 0) {
            memory_type_index = i;
            break;
        }
    }
    uint32_t memory_heap_index = memory_properties.memoryTypes[memory_type_index].heapIndex;
    VkDeviceSize memory_heap_size = memory_properties.memoryHeaps[memory_heap_index].size;
    return memory_heap_size;
}

void print_device_extensions_info(VkPhysicalDevice device) {
    uint32_t extension_count;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);
    std::vector<VkExtensionProperties> available_extesions(extension_count);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, available_extesions.data());

    std::cout << "\tAvailable device extensions(" << extension_count << "):\n";
    for (const auto& extension : available_extesions) {
        std::cout << "\t\t" << extension.extensionName << '\n';
    }
    std::cout << "\n\n";
}

void print_physical_device_info(const VkPhysicalDevice& device, bool selected = false) {
    VkPhysicalDeviceProperties device_properties;
    vkGetPhysicalDeviceProperties(device, &device_properties);

    std::string device_type_str;
    if (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        device_type_str = "DISCRETE_GPU";
    } else if (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
        device_type_str = "INTEGRATED_GPU";
    } else {
        device_type_str = "OTHER";
    }

    float vram_size_in_GB = (float)get_device_dedicated_vram_size(device) / (1024 * 1024 * 1024);

    std::cout << device_properties.deviceName << (selected ? " (*)" :  "") << '\n';
    std::cout << "\tdeviceID: " << device_properties.deviceID << '\n';
    std::cout << "\tdeviceType: " << device_type_str << '\n';
    std::cout << "\tvendorID: " << device_properties.vendorID << '\n';
    std::cout << "\tVRAM size: " << std::fixed << std::setprecision(1) << vram_size_in_GB  << " GB\n";
    print_device_extensions_info(device);
    std::cout << std::endl;
}

bool Application::is_suitable_device(VkPhysicalDevice _device) {
    bool flag = true;

    auto indices = find_queue_families(_device);
    flag &= indices.is_complete();
    flag &= check_device_extension_support(_device);

    if (flag) {
        auto swap_chain_support_details = query_swap_chain_support(_device);
        flag &= !swap_chain_support_details.formats.empty();
        flag &= !swap_chain_support_details.present_modes.empty();
    }

    return flag;
}

bool Application::check_device_extension_support(VkPhysicalDevice _device) {
    uint32_t extension_count;
    vkEnumerateDeviceExtensionProperties(_device, nullptr, &extension_count, nullptr);
    std::vector<VkExtensionProperties> available_extesions(extension_count);
    vkEnumerateDeviceExtensionProperties(_device, nullptr, &extension_count, available_extesions.data());

    for (auto requested_extension : required_device_extensions) {
        std::string requested_extension_str = requested_extension;
        bool extension_found = false;

        for (auto& extension : available_extesions) {
            if (requested_extension_str == extension.extensionName) {
                extension_found = true;
                break;
            }
        }

        if (!extension_found) return false;
    }

    return true;
}

void Application::sort_physical_devices(std::vector<VkPhysicalDevice>& devices) {
    std::sort(devices.begin(), devices.end(), [this] (const VkPhysicalDevice& deviceA, const VkPhysicalDevice& deviceB) {
        if (is_suitable_device(deviceA) != is_suitable_device(deviceB)) {
            return is_suitable_device(deviceA) > is_suitable_device(deviceB);
        }

        VkPhysicalDeviceProperties deviceA_properties, deviceB_properties;
        vkGetPhysicalDeviceProperties(deviceA, &deviceA_properties);
        vkGetPhysicalDeviceProperties(deviceB, &deviceB_properties);

        bool is_deviceA_preferred = (deviceA_properties.deviceType == PREFERRED_DEVICE_TYPE);
        bool is_deviceB_preferred = (deviceB_properties.deviceType == PREFERRED_DEVICE_TYPE);
        if (is_deviceA_preferred != is_deviceB_preferred) {
            return is_deviceA_preferred > is_deviceB_preferred;
        }

        auto deviceA_vram_size = get_device_dedicated_vram_size(deviceA);
        auto deviceB_vram_size = get_device_dedicated_vram_size(deviceB);
        return deviceA_vram_size > deviceB_vram_size;
    });
}

SwapChainSupportDetails Application::query_swap_chain_support(VkPhysicalDevice _device) {
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_device, surface, &details.capabilities);

    uint32_t format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(_device, surface, &format_count, nullptr);
    if (format_count != 0) {
        details.formats.resize(format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(_device, surface, &format_count, details.formats.data());
    }

    uint32_t present_mode_count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(_device, surface, &present_mode_count, nullptr);
    if (present_mode_count != 0) {
        details.present_modes.resize(present_mode_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(_device, surface, &present_mode_count, details.present_modes.data());
    }

    return details;
}

VkSurfaceFormatKHR Application::choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR>& formats) {
    for (const auto& format : formats) {
        if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return format;
        }
    }
    return formats.front();
}

VkPresentModeKHR Application::choose_swap_present_mode(const std::vector<VkPresentModeKHR>& present_modes) {
    for (const auto& present_mode : present_modes) {
        if (present_mode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return present_mode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D Application::choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    } else {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        VkExtent2D actual_extent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        actual_extent.width = std::clamp(actual_extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actual_extent.height = std::clamp(actual_extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actual_extent;
    }
}

std::vector<char> read_file(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open: " + filename);
    }

    size_t filesize = (size_t) file.tellg();
    std::vector<char> buffer(filesize);
    file.seekg(0);
    file.read(buffer.data(), filesize);
    file.close();
    return buffer;
}

VkShaderModule Application::create_shader_module(const std::vector<char>& code) {
    VkShaderModuleCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.codeSize = code.size();
    create_info.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shader_module;
    if (vkCreateShaderModule(device, &create_info, nullptr, &shader_module) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create shader info.");
    }

    return shader_module;
}

uint32_t Application::find_memory_type(uint32_t type_filter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memory_properties;
    vkGetPhysicalDeviceMemoryProperties(physical_device, &memory_properties);
    
    for (uint32_t i = 0; i < memory_properties.memoryTypeCount; ++i) {
        if ((type_filter & (1 << i)) && 
            (memory_properties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("Failed to find suitable memory type.");
}

#endif