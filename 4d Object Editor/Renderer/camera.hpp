//
//  camera.hpp
//  4d projection
//
//  Created by David on 20.09.22.
//

#ifndef camera_hpp
#define camera_hpp

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

//#define GLM_SWIZZLE
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/string_cast.hpp>

#include "imgui/imgui.h"

#include "math4d.hpp"


using namespace glm;


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);


struct CameraFlags {
    enum : uint32_t {
        recalculateProj = 1 << 0,
        orientUp        = 1 << 1,
    };
};

/// 4d camera to be used with OpenGL rendering
class Camera
{
public:
    /// Initializes camera at default pos with default view direction
    Camera();
    
    /// update certain internal parameters
    void update();
    /// move the camera according to the inputs and update matricies
    /// - Parameter window: window to listen to
    void handleInput(GLFWwindow* window);
    /// handles DearImGui interface
    void handleGUI();
    
    /// get pointer to view matrix
    const float* getViewPtr() const;
    /// get pointer to position vector
    const float* getPosPtr() const;
    /// get pointer to projection matrix
    const float* getProjPtr() const;
    
    /// set position of camera
    void setPos(vec4 pos);
    
    /// get position of camera
    vec4 getPos() const;
    /// get direction of ray under mouse
    vec4 getMouseRayDir() const;
    /// get forward direction
    vec4 getFwd() const;
    /// get direction perpendicular to view space
    vec4 getPerp() const;
    /// get world direction of screen up
    vec4 getUp() const;
    /// get world direction of screen right
    vec4 getRight() const;
    
    
    /// get the view coordinates of pos (horiz, depth, vert, perp)
    /// - Parameter pos: world position to be transformed
    vec4 getViewSpace(vec4 pos) const;

    /// callback for window resize
    void window_size_callback(GLFWwindow* window, int width, int height);
    
    
    /// get field of view of camera in radians
    float getFOV() const;
    /// get aspect ratio (width / height)
    float getAspectRatio() const;
private:
    void updateView();
    void orientUp();
    
    mat4 _view;
    mat4 _viewInverse;
    mat4 _proj;
    
    vec4 _pos {0, 0, 0, 0};
    
    vec4 _right     {1, 0, 0, 0};
    vec4 _up        {0, 1, 0, 0};
    vec4 _forward   {0, 0, 1, 0};
    vec4 _perp      {0, 0, 0, 1};
    
    float angleY=0;
    
    float _fov = 45;
    
    vec4 mouseDir {0, 0, 1, 0};
    
    double mouseX=0, mouseY=0;
    
    int window_w=1, window_h=1;
    
    // orient camera up
    bool makeUp = true;
    
    float _moveSpeed = 0.02;
    float _rotSpeed = 0.001;
    
    uint32_t _flags = 0;
    
    struct Keys {
        int
        k_front     = GLFW_KEY_W,
        k_back      = GLFW_KEY_S,
        k_left      = GLFW_KEY_A,
        k_right     = GLFW_KEY_D,
        k_up        = GLFW_KEY_Q,
        k_down      = GLFW_KEY_E,
        k_kata      = GLFW_KEY_F,
        k_ana       = GLFW_KEY_R;
        
    } _keyInfo;
};

#endif /* camera_hpp */
