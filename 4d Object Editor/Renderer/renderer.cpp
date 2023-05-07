//
//  renderer.cpp
//  4d projection
//
//  Created by David on 01.10.22.
//

#include "renderer.hpp"

void checkError()
{
    GLuint error = glGetError();
    if (error == GL_NO_ERROR) {
        return;
    }
    std::cout << "OpenGL Error:" << error << std::endl;
       
    
//        if (error == GL_INVALID_OPERATION) {
//            std::cout << "INVALID OPERATION\n";
//        }

}

FramebufferHandler::FramebufferHandler(int width, int height, GLuint sharedDepthTex)
: _width(width), _height(height)
{
    glGenFramebuffers(1, &_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
    
    glGenTextures(1, &_colorTex);
    glBindTexture(GL_TEXTURE_2D, _colorTex);
    glTexImage2D(GL_TEXTURE_2D, 0, _colorFormat, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    /*
    glGenRenderbuffers(1, &mainDepthbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, mainDepthbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, framebuffer_width, framebuffer_height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mainDepthbuffer);
    */
    
    if (sharedDepthTex != -1) {
        _depthTex = sharedDepthTex;
    } else {
        glGenTextures(1, &_depthTex);
        glBindTexture(GL_TEXTURE_2D, _depthTex);
        glTexImage2D(GL_TEXTURE_2D, 0, _depthFormat, _width, _height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        
        
        //glGenTextures(1, &depthTexture);
        //glBindTexture(GL_TEXTURE_2D, depthTexture);
        //glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT24, 1024, 768, 0,GL_DEPTH_COMPONENT, GL_FLOAT, 0);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _colorTex, 0);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _depthTex, 0);
    
//    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
//    glDrawBuffers(1, DrawBuffers);
    
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        printf("Framebuffer creation failed!\n");
    }
    
}

void FramebufferHandler::bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
    glViewport(0, 0, _width, _height);
}

void FramebufferHandler::bindWindow()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FramebufferHandler::resize(int width, int height)
{
    if (_width == width && _height == height) {
        return;
    }
    _width = width;
    _height = height;
    
    glBindTexture(GL_TEXTURE_2D, _colorTex);
    glTexImage2D(GL_TEXTURE_2D, 0, _colorFormat, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    
    glBindTexture(GL_TEXTURE_2D, _depthTex);
    glTexImage2D(GL_TEXTURE_2D, 0, _depthFormat, _width, _height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
}

GLuint FramebufferHandler::getColorTexID()
{
    return _colorTex;
}

GLuint FramebufferHandler::getDepthTexID()
{
    return _depthTex;
}



UniformHandler::UniformHandler(UniformType type, const char* gui_name, const char* var_name, vec4 initialData)
: _type(type), _var_name(var_name), _gui_name(gui_name), _data(initialData)
{
    
}

UniformHandler::UniformHandler(UniformType type, const char* gui_name, const char* var_name, float* dataPtr)
: _type(type), _var_name(var_name), _gui_name(gui_name), _storage(dataPtr)
{
    
}

void UniformHandler::setLoc(const Program &program)
{
    _location = program.getUniformLocation(_var_name);
    checkError();
}

void UniformHandler::handleGUI()
{
    if (_storage) {
        return;
    }
    switch (_type) {
        case UniformType::flt:
            ImGui::DragFloat(_gui_name, &_data.x);
            break;
            
        case UniformType::size:
            ImGui::DragFloat(_gui_name, &_data.x, 0.01, 0, FLT_MAX);
//            _data.x = max(_data.x, 0.0f);
            break;

        case UniformType::lsize:
            ImGui::DragFloat(_gui_name, &_data.x, 0.1, 0, 100, "%.5f", ImGuiSliderFlags_Logarithmic);
            break;
            
        case UniformType::ratio:
            ImGui::SliderFloat(_gui_name, &_data.x, 0, 1);
            break;
            
        case UniformType::pos2:
            ImGui::DragFloat2(_gui_name, &_data.x);
            break;
            
        case UniformType::pos3:
            ImGui::DragFloat3(_gui_name, &_data.x);
            break;
            
        case UniformType::pos4:
            ImGui::DragFloat4(_gui_name, &_data.x);
            break;
            
        case UniformType::dir4:
            ImGui::SliderFloat4(_gui_name, &_data.x, -1, 1);
            break;
            
        case UniformType::color3:
            ImGui::ColorEdit3(_gui_name, &_data.x);
            break;
            
        case UniformType::color4:
            ImGui::ColorEdit4(_gui_name, &_data.x);
            break;
        
        case UniformType::boolean:
            ImGui::Checkbox(_gui_name, (bool*)&_data.x);
            break;
            
        case UniformType::enumeration:
            ImGui::SliderInt(_gui_name, (int*)&_data.x, 0, 16);
            break;
            
        default:
            throw;
            break;
    }
}

void UniformHandler::setUniform()
{
    float* loc = _storage;
    if (!_storage) {
        loc = value_ptr(_data);
    }
    switch (_type) {
        case UniformType::flt:
        case UniformType::size:
        case UniformType::lsize:
        case UniformType::ratio:
            glUniform1fv(_location, 1, loc);
            break;
            
        case UniformType::pos2:
            glUniform2fv(_location, 1, loc);
            break;
            
        case UniformType::pos3:
        case UniformType::color3:
            glUniform3fv(_location, 1, loc);
            break;
            
        case UniformType::pos4:
        case UniformType::color4:
        case UniformType::dir4:
            glUniform4fv(_location, 1, loc);
            break;
        case UniformType::boolean:
        case UniformType::enumeration:
            glUniform1iv(_location, 1, (int*)loc);
            break;
        default:
            throw;
            break;
    }
}

uint32_t ShaderController::makeFlag(bool blend, bool depthTest, bool writeDepth)
{
    uint32_t flag = 0;
    if (blend) {
        flag |= RENDER_FLAG_BLEND;
    }
    if (depthTest) {
        flag |= RENDER_FLAG_DEPH_TEST;
    }
    if (writeDepth) {
        flag |= RENDER_FLAG_DEPH_WRITE;
    }
    return flag;
}



ShaderController::ShaderController(const Program& shader, RenderMode mode, const std::vector<UniformHandler>& uniforms,uint32_t flags)
: _uniforms(uniforms)
{    
    _shader = shader;
    _flags = flags;
    
    _mode = mode;
    switch (mode) {
        case RenderMode::vertecies:
            _primitive = GL_POINTS;
            break;
        case RenderMode::edges:
            _primitive = GL_LINES;
            break;
        case RenderMode::faces:
            _primitive = GL_TRIANGLES;
            break;
        case RenderMode::cells:
            _primitive = GL_LINES_ADJACENCY;
            break;
        default:
            throw;
            break;
    }
    
    setLocations();
}

void ShaderController::setTransformUniform(const GLfloat *position, const GLfloat *transform) {
    setPosition(position);
    setTransform(transform);
}

void ShaderController::render(GLuint VAO, int count)
{
//    START_TIMER("set uniform");
    
//    END_TIMER("set uniform");

//    START_TIMER("bind VAO");
    glBindVertexArray(VAO);
//    END_TIMER("bind VAO");

    START_TIMER(shaderNames[(int)_mode]);
    if (_mode == RenderMode::vertecies) {
        glDrawArrays(_primitive, 0, count);
        checkError();
    } else {
        glDrawElements(_primitive, count, GL_UNSIGNED_INT, 0);
        checkError();
    }
    END_TIMER(shaderNames[(int)_mode]);
}

void ShaderController::multirender(multidrawInfo info)
{
    mat4 position{1};
    vec4 transform{0};
    setPosition(value_ptr(position));
    setTransform(value_ptr(transform));
    
//    END_TIMER("set uniform");

//    START_TIMER("bind VAO");
    
    throw ;
//    glBindVertexArray(VAO);
////    END_TIMER("bind VAO");
//
//    START_TIMER(shaderNames[(int)_mode]);
//    if (_mode == RenderMode::vertecies) {
//        glDrawArrays(_primitive, 0, count);
//        checkError();
//    } else {
//        glDrawElements(_primitive, count, GL_UNSIGNED_INT, 0);
//        checkError();
//    }
//    END_TIMER(shaderNames[(int)_mode]);
}


void ShaderController::prepareDraw(const GLfloat* viewPtr, const GLfloat* offPtr, const GLfloat* projPtr)
{
    activate();

    setFlags();
    
    setUniforms();
//    setTextures();

    setView(viewPtr);
    setOffset(offPtr);
    setProjection(projPtr);
    
}

void ShaderController::setFlags()
{
    if (_flags & RENDER_FLAG_BLEND) {
        glEnable(GL_BLEND);
    } else {
        glDisable(GL_BLEND);
    }
    if (_flags & RENDER_FLAG_DEPH_TEST) {
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST);
    }
    if (_flags & RENDER_FLAG_DEPH_WRITE) {
        glDepthMask(true);
    } else {
        glDepthMask(false);
    }
    
    _shader.activate();
}

void ShaderController::activate()
{
    _shader.activate();
}

void ShaderController::setLocations()
{
    _viewLoc = _shader.getUniformLocation("view");
    _offsetLoc = _shader.getUniformLocation("offset");
    _projectionLoc = _shader.getUniformLocation("proj");
    _positionLoc = _shader.getUniformLocation("position");
    _transformLoc = _shader.getUniformLocation("transform");
    _useTexLoc = _shader.getUniformLocation("useTex");
    
    for (UniformHandler& uniform : _uniforms) {
        uniform.setLoc(_shader);
    }
}

inline void ShaderController::setView(const GLfloat* viewPtr)
{
    glUniformMatrix4fv(_viewLoc, 1, GL_FALSE, viewPtr);
}


void ShaderController::setOffset(const GLfloat* offPtr)
{
    glUniform4fv(_offsetLoc, 1, offPtr);
}


void ShaderController::setProjection(const GLfloat* projPtr)
{
    glUniformMatrix4fv(_projectionLoc, 1, GL_FALSE, projPtr);
}

void ShaderController::setPosition(const GLfloat* posPtr)
{
    glUniform4fv(_positionLoc, 1, posPtr);
}

void ShaderController::setTransform(const GLfloat* transformPtr)
{
    glUniformMatrix4fv(_transformLoc, 1, GL_FALSE, transformPtr);
}

void ShaderController::setUseTexture(bool useTex)
{
    glUniform1i(_useTexLoc, useTex);
}

void ShaderController::setUniforms()
{
    for (int i = 0; i < _uniforms.size(); i++) {
        _uniforms[i].setUniform();
    }
}

void ShaderController::setTextures()
{
//    for (int i = 0; i < _textures.size(); i++) {
//        _textures[i].bind();
//    }
}

void ShaderController::addUniform(const UniformHandler &uniform)
{
    _uniforms.push_back(uniform);
    activate();
    _uniforms.back().setLoc(_shader);
}

void ShaderController::addTextureHandler(const TextureUniformHandler &tuh)
{
    _textures.push_back(tuh);
    activate();
    _textures.back().setLoc(_shader);
}

void ShaderController::handleGUI()
{
    ImGui::PushID(this);
    for (UniformHandler& uniform : _uniforms) {
        uniform.handleGUI();
    }
    ImGui::PopID();
}


RenderController::RenderController(const ShaderController& vertecies, const ShaderController& edges, const ShaderController& faces, const ShaderController& slice, RenderMode mode) :
_vertecies(vertecies),
_edges(edges),
_faces(faces),
_slice(slice)
{
    _active = &_slice;
    _mode = RenderMode::disable;
}

RenderController::RenderController()
{
    
}

void RenderController::render()
{
    prepareDraw();
    
    checkError();
    

    _enabled_count = 0;
    for (int i = 0; i < _meshes.size(); i++) {
        if (_meshes[i]->_flags & _meshDisableFlags) {
            continue;
        }
        _enabled_count++;
        setMeshUniforms(_meshes[i]);
        renderSingle(_meshes[i]);

    }
    {
        mat4 trans{1};
        vec4 offset{0, 0, 0, 0};
        _active->setTransformUniform(value_ptr(offset), value_ptr(trans));
    }
    for (int i = 0; i < _tempMeshes.size(); i++) {
        if (_tempMeshes[i]->_flags & _meshDisableFlags) {
            continue;
        }
        _enabled_count++;
        renderSingle(_tempMeshes[i]);

    }
}

void RenderController::submitTempMesh(Mesh *mesh)
{
    _tempMeshes.push_back(mesh);
}

void RenderController::clearTempMesh()
{
    _tempMeshes.clear();
}


void RenderController::prepareDraw()
{
    _active->prepareDraw(_viewPtr, _offPtr, _projPtr);
}

void RenderController::setMeshUniforms(const Mesh* mesh)
{
    _active->setTransformUniform(mesh->getPos(), mesh->getTransform());
    _active->setUseTexture(mesh->_tex);
    if (mesh->_tex) {
        assert(_active->_textures.size() == 1);
        
        _active->_textures[0].bind(mesh->_tex);
    }
    
}

void RenderController::renderSingle(const Mesh *mesh)
{
    _active->render(mesh->getVAO(_mode),
                    mesh->getDrawCount(_mode));
}


void RenderController::addMesh(Mesh *mesh)
{
    _meshes.push_back(mesh);
}

void RenderController::setMode(RenderMode mode)
{
    if (mode == _mode) {
        return;
    }
    _mode = mode;
    switch (mode) {
        case RenderMode::vertecies:
            _active = &_vertecies;
            _meshDisableFlags = MeshFlags::disable | MeshFlags::disableVert;
//            _colorPtr = value_ptr(_settings.vertexColor);
            break;
        case RenderMode::edges:
            _active = &_edges;
            _meshDisableFlags = MeshFlags::disable | MeshFlags::disableEdge;
//            _colorPtr = value_ptr(_settings.edgeColor);
            break;
        case RenderMode::faces:
            _active = &_faces;
            _meshDisableFlags = MeshFlags::disable | MeshFlags::disableFace;
//            _colorPtr = value_ptr(_settings.faceColor);
            break;
        case RenderMode::cells:
            _active = &_slice;
            _meshDisableFlags = MeshFlags::disable | MeshFlags::disableTetra;
//            _colorPtr = value_ptr(_settings.cellColor);
            break;
        default:
            throw;
    }
}

void RenderController::setPtr(const GLfloat *viewPtr, const GLfloat *offPtr, const GLfloat *projPtr)
{
    _viewPtr = viewPtr;
    _offPtr = offPtr;
    _projPtr = projPtr;
}

//const std::vector<Mesh*>& RenderController::getEditableMeshes()
//{
//    return _meshes;
//}

void RenderController::handleGUI()
{
    ImGui::Begin("renderer settings");
    
    _vertecies.handleGUI();
    _edges.handleGUI();
    _faces.handleGUI();
    _slice.handleGUI();

    
    ImGui::Text("Meshes: %lu", _meshes.size());
    ImGui::Text("Enabled Meshes: %d", _enabled_count);

    ImGui::End();
}

RenderPassController::RenderPassController(const std::vector<RenderPassOperation>& operations,
                                           const std::vector<FramebufferInitMode>& fb_inits)
:_operations(operations), _framebuffers()
{
    for (auto fim : fb_inits) {
        GLuint sharedTex = -1;
        if (fim.sharedDepthBufferIndex != -1) {
            sharedTex = _framebuffers[fim.sharedDepthBufferIndex].getDepthTexID();
        }
        FramebufferHandler h {0, 0, sharedTex};
        _framebuffers.push_back(h);
    }
}

void RenderPassController::draw(RenderMode mode)
{
    // not implemented
    throw;
}

void RenderPassController::execute(RenderPassOperation op)
{
    switch (op.operation) {
        case RenderPassOperation::none:
            break;
        
        case RenderPassOperation::draw:
            draw(op.data.drawMode);
            break;

        case RenderPassOperation::clear:
            glClearColor(op.data.clearColor.x,
                         op.data.clearColor.y,
                         op.data.clearColor.z,
                         op.data.clearColor.w);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            break;
            
        case RenderPassOperation::change_render_target:
            _framebuffers[op.data.renderTarget].bind();
            break;
    }
}
