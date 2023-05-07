//
//  window.cpp
//  4d Object Editor
//
//  Created by David on 29.03.23.
//

#include "window.hpp"

void Window::initialize()
{
    if (_init) {
        return;
    }
    if (!glfwInit()) {
        return;
    }
    
    _init = true;
}


Window::Window(int width, int height, const char* title)
:_width(width), _height(height)
{
    initialize();
    
    _window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    
    if (_window == nullptr) {
        printf("Failed to create GLFW window\n");
    }
}

void Window::close()
{
    glfwDestroyWindow(_window);
}

int Window::width() const
{
    return _width;
}

int Window::height() const
{
    return _height;
}

int Window::getFbWidth() const
{
    return _fb_width;
}

int Window::getFbHeight() const
{
    return _fb_height;
}

GLFWwindow* Window::getWindow()
{
    return _window;
}

bool Window::shouldClose()
{
    return glfwWindowShouldClose(_window);
}

void Window::update()
{
    glfwPollEvents();
    
    glfwGetWindowSize(_window, &_width, &_height);
    glfwGetFramebufferSize(_window, &_fb_width, &_fb_height);
}

int Window::getKey(int key)
{
    return glfwGetKey(_window, key);
}


void Window::makeContextCurrent()
{
    glfwMakeContextCurrent(_window);
}

void Window::swapBuffers()
{
    glfwSwapBuffers(_window);
}
