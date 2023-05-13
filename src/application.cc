#include "application.h"

const uint32_t window_width = 800;
const uint32_t window_height = 600;

Application::Application() {
    init_glfw();
}

void Application::run() {
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }
}

Application::~Application() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Application::init_glfw() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(window_width, window_height, "hello-triangle", nullptr, nullptr);
}
