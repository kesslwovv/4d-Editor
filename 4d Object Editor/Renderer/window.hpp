//
//  window.hpp
//  4d Object Editor
//
//  Created by David on 29.03.23.
//

#ifndef window_hpp
#define window_hpp

#include <stdio.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>


class Window
{
public:
    Window() = default;
    Window(int width, int height, const char* title);
    
    void close();
    
    int width() const;
    int height() const;
    int getFbWidth() const;
    int getFbHeight() const;
    
    GLFWwindow* getWindow();
    
    void makeContextCurrent();
    void swapBuffers();
    bool shouldClose();
    void update();
    
    int getKey(int key);
private:
    GLFWwindow* _window = nullptr;
    inline static bool _init = false;
    static void initialize();
    int _width=0, _height=0;
    int _fb_width=0, _fb_height=0; // framebuffer size may be different from window size
};

#endif /* window_hpp */
