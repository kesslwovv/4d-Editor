//
//  shaderProgram.hpp
//  4d projection
//
//  Created by David on 19.09.22.
//

#ifndef shaderProgram_hpp
#define shaderProgram_hpp

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <fstream>
#include <iostream>
#include <filesystem>

#include "utils.hpp"

using std::string;

/// Wrapper for OpenGL (shader) Programs
class Program
{
public:
    Program() = default;
    
    /// read shader code from files and compile shader
    /// - Parameters:
    ///   - vertexFile: vertex shader source file, relative to executable directory
    ///   - fragmentFile: fragment shader source file, relative to executable directory
    Program(const char* vertexFile, const char* fragmentFile, const char* geometryFile = nullptr);
    
    /// set program as the active (bound by OpenGL) program
    void activate();
    
    /// Get the location of uniform shader variable
    /// - Parameter var: name of variable in shader
    GLuint getUniformLocation(const char* var) const;
    
    /// Destroy shaders and associated OpenGL resources
    void destroy();
    
    GLuint program=0;
};



#endif /* shaderProgram_hpp */
