//
//  camera.cpp
//  4d projection
//
//  Created by David on 20.09.22.
//

#include "camera.hpp"

#define NORM(vec) vec = normalize(vec)

static int* callbackTarget = nullptr;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (!callbackTarget) {
        return;
    }
    
//    if (action == GLFW_PRESS) {
        *callbackTarget = key;
        callbackTarget = nullptr;
//    }
    
}

Camera::Camera()
{
    window_w = 1;
    window_h = 1;
    _proj = glm::perspective(glm::radians(_fov), float(window_w) / window_h, 0.01f, 100.0f);
    
    _pos = vec4(0, 0, -3, 0);

    _right   = vec4(1, 0, 0, 0);
    _up      = vec4(0, 1, 0, 0);
    _forward = vec4(0, 0, 1, 0);
    _perp    = vec4(0, 0, 0, 1);
    
    updateView();
}

void Camera::updateView()
{
    //? inverse forward because opengl z is negative?
    _viewInverse = mat4(_right, _up, -_forward, _perp);
    _view = inverse(_viewInverse);
}

void Camera::window_size_callback(GLFWwindow *window, int width, int height)
{
    
    _flags |= CameraFlags::recalculateProj;
}

void Camera::orientUp() {
    // orient the camera up to avoid rolling
    float angle = atan(_right.y);
    rotatePlane(_right, _up, _right, _up, -angle);
}

void Camera::update()
{
    // clear _flags
    if (_flags & CameraFlags::recalculateProj) {
        _proj = glm::perspective(glm::radians(_fov),
                                 float(window_w) / window_h,
//                                 1.0f,
                                 0.01f, 100.0f);
        _flags &= ~CameraFlags::recalculateProj;
    }
    
    if (_flags & CameraFlags::orientUp) {
        orientUp();
        if (abs(_right.y) < 0.0001) {
            _flags &= ~CameraFlags::orientUp;
        }
    }
}

void Camera::handleInput(GLFWwindow* window)
{
    glfwGetWindowSize(window, &window_w, &window_h);
    
    if (glfwGetKey(window, _keyInfo.k_front) == GLFW_PRESS)
    {
        _pos += _moveSpeed * _forward;
    }
    if (glfwGetKey(window, _keyInfo.k_left) == GLFW_PRESS)
    {
        _pos += _moveSpeed * -_right;
    }
    if (glfwGetKey(window, _keyInfo.k_back) == GLFW_PRESS)
    {
        _pos += _moveSpeed * -_forward;
    }
    if (glfwGetKey(window, _keyInfo.k_right) == GLFW_PRESS)
    {
        _pos += _moveSpeed * _right;
    }
    if (glfwGetKey(window, _keyInfo.k_up) == GLFW_PRESS)
    {
        _pos += _moveSpeed * _up;
    }
    if (glfwGetKey(window, _keyInfo.k_down) == GLFW_PRESS)
    {
        _pos += _moveSpeed * -_up;
    }
    if (glfwGetKey(window, _keyInfo.k_ana) == GLFW_PRESS)
    {
        _pos += _moveSpeed * _perp;
    }
    if (glfwGetKey(window, _keyInfo.k_kata) == GLFW_PRESS)
    {
        _pos += _moveSpeed * -_perp;
    }
    
    double mouseXprev=mouseX, mouseYprev=mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);
    
    if (glfwGetKey(window, GLFW_KEY_SPACE)) {
        float rotX = _rotSpeed * (mouseX - mouseXprev);
        float rotY = _rotSpeed * (mouseY - mouseYprev);
        
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            //            if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            rotatePlane(_right, _perp, rotX);
            
            angleY = clamp(angleY + rotY, -1.5f, 1.5f);
            
            rotatePlane(_perp, _up, _perp, _up, rotY);
            //            } else {
        } else {
            rotatePlane(_right, _forward, rotX);
            
            angleY = clamp(angleY + rotY, -1.5f, 1.5f);
            
            rotatePlane(_forward, _up, rotY);
            //            }
        }
            
        NORM(_forward);
        NORM(_right);
        NORM(_up);
        NORM(_perp);
            
        if (makeUp && abs(_right.y) > 0.000001) {
            orientUp();
//        }
        }
        mouseX = mouseXprev;
        mouseY = mouseYprev;
        
        printf("cursor: %f %f\n", mouseX, mouseY);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetCursorPos(window, mouseXprev, mouseYprev);
        
    } else {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    mouseDir = _forward
                + _right * (float)(mouseX/window_w*2 - 1) * tan(radians(_fov)/2) * (float)window_w / (float)window_h
                - _up * (float)(mouseY/window_h*2 - 1) * tan(radians(_fov)/2);
    
    NORM(mouseDir);
    
    updateView();
}

const float* Camera::getViewPtr() const
{
    return value_ptr(_view);
}

const float* Camera::getPosPtr() const
{
    return value_ptr(_pos);
}

const float* Camera::getProjPtr() const
{
    return value_ptr(_proj);
}

vec4 Camera::getPos() const
{
    return _pos;
}

vec4 Camera::getMouseRayDir() const
{
    return mouseDir;
}

vec4 Camera::getPerp() const
{
    return _perp;
}

vec4 Camera::getFwd() const
{
    return _forward;
}

vec4 Camera::getUp() const
{
    return _up;
}

vec4 Camera::getRight() const
{
    return _right;
}

vec4 Camera::getViewSpace(vec4 pos) const
{
    return _view * (pos - _pos);
}

float Camera::getFOV() const
{
    return radians(_fov);
}

float Camera::getAspectRatio() const
{
    return (float)window_w / window_h;
}

void Camera::setPos(vec4 pos)
{
    _pos = pos;
}

void Camera::handleGUI()
{
    ImGui::Begin("Camera info");
    
    ImGui::DragFloat4("position", value_ptr(_pos));
    
    if (ImGui::Checkbox("orient up", &makeUp)) {
        _flags |= CameraFlags::orientUp;
    }
    
    ImGui::DragFloat("move speed", &_moveSpeed, 0.1, 0, 100, "%.4f", ImGuiSliderFlags_Logarithmic);
    ImGui::SliderFloat("rotation speed", &_rotSpeed, 0, 0.01);
    
    if (ImGui::SliderFloat("FOV", &_fov, 10, 160)) {
        _flags |= CameraFlags::recalculateProj;
    }
    
//    if (ImGui::Button("recalculate _proj")) {
//        _flags |= CameraFlags::recalculateProj;
//    }
    
    if (ImGui::TreeNode("controls")) {
        if (ImGui::Button("forward")) {callbackTarget = &_keyInfo.k_front;}
        ImGui::SameLine();
        ImGui::Text("%s", glfwGetKeyName(_keyInfo.k_front, 0));

        if (ImGui::Button("backward")) {callbackTarget = &_keyInfo.k_back;}
        ImGui::SameLine();
        ImGui::Text("%s", glfwGetKeyName(_keyInfo.k_back, 0));
        
        ImGui::TreePop();
    }
    
    if (ImGui::TreeNode("directions")) {
        ImGui::BeginDisabled();
        ImGui::DragFloat4("forward", value_ptr(_forward));
        ImGui::DragFloat4("up", value_ptr(_up));
        ImGui::DragFloat4("right", value_ptr(_right));
        ImGui::DragFloat4("perpendicular", value_ptr(_perp));
        ImGui::EndDisabled();
        ImGui::TreePop();
    }
    
    if (ImGui::TreeNode("reset orientation")) {
        if (ImGui::Button("XYZW")) {
            _forward =   vec4(0, 0, 1, 0);
            _up =        vec4(0, 1, 0, 0);
            _right =     vec4(1, 0, 0, 0);
            _perp =      vec4(0, 0, 0, 1);
            updateView();
        }
        
        if (ImGui::Button("ZYWX")) {
            _forward =   vec4(0, 0, 0, 1);
            _up =        vec4(0, 1, 0, 0);
            _right =     vec4(0, 0, 1, 0);
            _perp =      vec4(1, 0, 0, 0);
            updateView();
        }
        
        if (ImGui::Button("WYXZ")) {
            _forward =   vec4(1, 0, 0, 0);
            _up =        vec4(0, 1, 0, 0);
            _right =     vec4(0, 0, 0, 1);
            _perp =      vec4(0, 0, 1, 0);
            updateView();
        }
        
        ImGui::TreePop();
    }
    
    ImGui::End();
}

