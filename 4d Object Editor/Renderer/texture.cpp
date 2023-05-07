//
//  texture.cpp
//  4d Object Editor
//
//  Created by David on 22.03.23.
//

#include "texture.hpp"

Texture::Texture(GLenum target)
: _target(target)
{
    genTexture();
}

Texture::Texture(path file)
{
    genTexture();
    readFileBinary(file);
}

void Texture::genTexture()
{
    glGenTextures(1, &_texID);
}

void Texture::setBinding(int bindPoint)
{
    assert(bindPoint >= 0);
    glActiveTexture(GL_TEXTURE0 + bindPoint);
    bind();
}

void Texture::bind()
{
    glBindTexture(_target, _texID);
}


void Texture::updateContent(void* data, GLenum dataFormat, GLenum dataType,
                            int width, int height, int depth)
{
    bind();
    if (height == 0) {
        glTexImage1D(_target, 0, _imgFormat, width, 0, dataFormat, dataType, data);
//        return;
    }
    if (depth == 0) {
        glTexImage2D(_target, 0, _imgFormat, width, height, 0, dataFormat, dataType, data);
//        return;
    } else {
        glTexImage3D(_target, 0, _imgFormat, width, height, depth, 0, dataFormat, dataType, data);
    }
    glTexParameteri(_target, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(_target, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(_target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void Texture::readFileBinary(path filename)
{
    std::ifstream file {abspath(filename)};
    TextureDataFormat texType;
    size_t w, h, d;
    file.read((char*)&texType, 8);
    
    file.read((char*)&w, 8);
    file.read((char*)&h, 8);
    file.read((char*)&d, 8);
    
    assert(texType == TextureDataFormat::float4);
    
    size_t length = w * h * d;
    
    std::vector<float> data{};
    data.resize(length*4);
    file.read((char*)&data[0], length*BYTES_PER_COLOR);
    
    updateContent(&data[0], GL_RGBA, GL_FLOAT, (int)w, (int)h, (int)d);
}


GLuint Texture::getID()
{
    return _texID;
}

TextureUniformHandler::TextureUniformHandler(int bindPoint, const char* name)
: _bindPoint(bindPoint), _name(name)
{
    
}

void TextureUniformHandler::setLoc(const Program &program)
{
    _uniformLoc = program.getUniformLocation(_name);
}

void TextureUniformHandler::bind(Texture* tex)
{
    if (!tex) {
        return;
    }
    glUniform1i(_uniformLoc, _bindPoint);
    tex->setBinding(_bindPoint);
}

