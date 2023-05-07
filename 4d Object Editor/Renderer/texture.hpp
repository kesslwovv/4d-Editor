//
//  texture.hpp
//  4d Object Editor
//
//  Created by David on 22.03.23.
//

#ifndef texture_hpp
#define texture_hpp

#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <filesystem>
#include <fstream>

#include "shaderProgram.hpp"

#define BYTES_PER_COLOR 16

enum class TextureDataFormat : uint64_t {
    invalid = 0,
    float4 = 1,
};

using std::filesystem::path;

/// Wrapper for OpenGl Texture
class Texture {
public:
    Texture() = default;
    explicit Texture(GLenum target);
    explicit Texture(path file);
    
    /// bind texture
    void bind();
    /// update data
    /// - Parameters:
    ///   - data: pointer to data
    ///   - dataFormat: Format of data (used by OpenGl)
    ///   - dataType: Type of Data (used by OpenGl)
    ///   - width: width of texture
    ///   - height: height of texture
    ///   - depth: depth of texture
    void updateContent(void* data, GLenum dataFormat, GLenum dataType,
                       int width, int height, int depth);
    /// Bind Texture to bind point
    /// - Parameter texLoc: Id of bind point
    void setBinding(int bindPoint);
    
    void readFileBinary(path filename);
    
    GLuint getID();
private:
    void genTexture();
    
    GLuint _texID = -1;
    GLenum _target=GL_TEXTURE_3D;
    GLenum _imgFormat=GL_RGBA;
};

class TextureUniformHandler {
public:
    TextureUniformHandler() = default;
    TextureUniformHandler(int bindPoint, const char* name);
    
    void setLoc(const Program& program);
    void bind(Texture* tex);
private:
    GLuint _uniformLoc=-1;
    int _bindPoint = -1;
    const char* _name = "";
};

#endif /* texture_hpp */
