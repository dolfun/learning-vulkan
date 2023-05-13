#ifndef DEVICE_UTILS_H_INCLUDED
#define DEVICE_UTILS_H_INCLUDED

#include <vulkan/vulkan.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>

#define PREFERRED_DEVICE_TYPE VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU

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

void print_physical_device_info(const VkPhysicalDevice& device) {
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

    std::cout << device_properties.deviceName << '\n';
    std::cout << "\tdeviceID: " << device_properties.deviceID << '\n';
    std::cout << "\tdeviceType: " << device_type_str << '\n';
    std::cout << "\tvendorID: " << device_properties.vendorID << '\n';
    std::cout << "\tVRAM size: " << std::fixed << std::setprecision(1) << vram_size_in_GB  << " GB\n";
    std::cout << std::endl;
}

bool is_suitable_device(const VkPhysicalDevice& device) {
    return true;
}

void sort_physical_devices(std::vector<VkPhysicalDevice>& devices) {
    std::sort(devices.begin(), devices.end(), [] (const VkPhysicalDevice& deviceA, const VkPhysicalDevice& deviceB) {
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

#endif