#ifndef APPLICATION_H_INCLUDED
#define APPLICATION_H_INCLUDED

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

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
    GLFWwindow* window;
    void init_glfw();
};

#endif