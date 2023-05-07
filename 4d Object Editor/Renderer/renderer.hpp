//
//  renderer.hpp
//  4d projection
//
//  Created by David on 01.10.22.
//

#ifndef renderer_hpp
#define renderer_hpp

#include <vector>


#include "glm/gtc/type_ptr.hpp"

#include "imgui/imgui.h"

// shaderProgram includes glad and GLFW
#include "shaderProgram.hpp"
#include "mesh.hpp"
#include "timer.hpp"

#include "texture.hpp"

void checkError();


/// Defines the different type of uniforms and how to input them
enum class UniformType {
    none,

    flt,   // any float
    size,  // positive
    lsize, // positive logarithmic
    ratio, // float [0 to 1]
    
    pos2,  // vec2
    pos3,  // vec3
    pos4,  // vec4
    
    dir4,  // direction [-1, 1]
    
    color3, // color
    color4, // color with alpha
    
    boolean,     // bool
    enumeration, // int 0-16
};

// used by timers
constexpr const char* shaderNames[] {
    "",
    "vertecies",
    "edges",
    "faces",
    "cells",
    "disable"
};

/// currently unused
struct multidrawInfo {
    GLenum mode;
    const GLsizei * count;
    GLenum type;
    const void * const * indices;
    GLsizei drawcount;
};

/// OpenGL framebuffer
class FramebufferHandler {
public:
    FramebufferHandler() = default;
    /// Initialize with width and height and (optional) shared tepth texture
    /// - Parameters:
    ///   - width: width of framebuffer
    ///   - height: height of framebuffer
    ///   - sharedDepthTex: depth texture to bind to if -1 new texture is created
    FramebufferHandler(int width, int height, GLuint sharedDepthTex=-1);
    
    /// set size of framebuffer and update if changed
    /// - Parameters:
    ///   - width: new framebuffer width
    ///   - height: new framebuffer height
    void resize(int width, int height);
    
    /// bind framebuffer for next OpenGL draw calls
    void bind();
    /// bind the (screen) window framebuffer
    static void bindWindow();
    
    /// get the ID of the color Texture
    GLuint getColorTexID();
    /// get the ID of the depth Texture (use for shared depth in framebuffers)
    GLuint getDepthTexID();
private:
    int _width=0, _height=0;
    GLuint _fbo=-1;
    GLuint _colorTex=-1;
    GLuint _depthTex=-1;
    
    GLenum _colorFormat = GL_RGBA16F;
    GLenum _depthFormat = GL_DEPTH_COMPONENT24;
};

class UniformHandler {
public:
    /// handles uniforms for ShaderController
    /// - Parameters:
    ///   - type: type of value (see `UniformType`)
    ///   - gui_name: name in the GUI
    ///   - var_name: name used by the shader
    ///   - initialData: initial data for vector types
    ///   - dataPtr: pointer to data to be used
    UniformHandler(UniformType type, const char* gui_name, const char* var_name, vec4 initialData = vec4(0));
    UniformHandler(UniformType type, const char* gui_name, const char* var_name, float* dataPtr);

    void setLoc(const Program& program);
    void handleGUI();
    void setUniform();
private:
    UniformType _type;
    GLuint _location{0};
    float* _storage=nullptr;
    bool _show=true;
    vec4 _data{0};
    const char* _var_name;
    const char* _gui_name;
};

/// handle a shader with all uniforms
class ShaderController
{
public:
    ShaderController() = default;
    /// Initialize Shader Controller
    /// - Parameters:
    ///   - shader: shader to be controlled
    ///   - mode: mode to be rendererd
    ///   - uniforms: uniform handlers for the shader
    ///   - flags: render flags
    ShaderController(const Program& shader, RenderMode mode, const std::vector<UniformHandler>& uniforms, uint32_t flags);
    
    /// set the per-mesh transform uniforms
    /// - Parameters:
    ///   - position: pointer to position vector
    ///   - transform: pointer to transform matrix vector
    void setTransformUniform(const GLfloat *position, const GLfloat *transform);
    
    /// render a VAO (needs rework)
    /// - Parameters:
    ///   - VAO: VAO
    ///   - count: vertex count
    void render(GLuint VAO, int count);
    /// unused
    /// - Parameter info:
    void multirender(multidrawInfo info);
    
    /// deprecated
    /// - Parameters:
    ///   - VAO: VAO
    ///   - index: index in VAO
    void renderSingle(GLuint VAO, int index, vec4 position=vec4(0));
    
    /// prepare for Drawing
    /// - Parameters:
    ///   - viewPtr: pointer to view matrix data
    ///   - offPtr: pointer to offset vector data
    ///   - projPtr: pointer to projection matrix data
    void prepareDraw(const GLfloat* viewPtr, const GLfloat* offPtr, const GLfloat* projPtr);
    
    /// set OpenGL settings based on flags
    void setFlags();
    /// activate shader
    void activate();
    /// set the respective uniforms
    /// - Parameter ...Ptr: pointer to ... vector / matrix data
    void setView(const GLfloat* viewPtr);
    void setOffset(const GLfloat* offPtr);
    void setProjection(const GLfloat* projPtr);
    void setPosition(const GLfloat* posPtr);
    void setTransform(const GLfloat* transformPtr);
    void setUseTexture(bool useTex);
    
    /// set all custom uniforms
    void setUniforms();
    /// set main texture
    void setTextures();
    
    /// append uniform to list
    /// - Parameter uniform: uniform to be added
    void addUniform(const UniformHandler& uniform);
    /// add uniform handler (rn call exactly once)
    /// - Parameter tuh: uniform handler for controlling a uniform
    void addTextureHandler(const TextureUniformHandler& tuh);
    
    /// load uniform locations from shader (call once)
    void setLocations();
    
    /// handle the GUI of this and all uniforms
    void handleGUI();
    
    /// Make a flag used for the init call
    /// - Parameters:
    ///   - blend: enable blending
    ///   - depthTest: enable depth testing
    ///   - depthWrite: enable depth writing
    static uint32_t makeFlag(bool blend, bool depthTest, bool depthWrite);
    RenderMode _mode;
    Program _shader;
    
    std::vector<TextureUniformHandler> _textures {};
private:
    GLenum _primitive;
    
    std::vector<UniformHandler> _uniforms {};
    
    TextureUniformHandler _tex {};
    
    GLuint  _viewLoc=-1,
            _offsetLoc=-1,
            _projectionLoc=-1,
            _positionLoc=-1,
            _transformLoc=-1,
            _useTexLoc=-1;
    
    uint32_t _flags;
#define RENDER_FLAG_BLEND 1
#define RENDER_FLAG_DEPH_TEST 2
#define RENDER_FLAG_DEPH_WRITE 4
};


class RenderController
{
public:
    RenderController();
    
    /// Initialize RenderController
    /// - Parameters:
    ///   - vertecies: vertex shader controller
    ///   - edges: edge shader controller
    ///   - faces: face shader controller
    ///   - slice: cell shader controller
    ///   - mode: render mode to start out with
    RenderController(const ShaderController& vertecies, const ShaderController& edges, const ShaderController& faces, const ShaderController& slice, RenderMode mode=RenderMode::cells);
    //    void setup();
    //    void handleInputs(GLFWwindow* window);
    
    /// render all meshes
    void render();
    
    /// (internal) prepare draw for rendering
    void prepareDraw();
    /// render a single mesh
    /// - Parameter mesh: mesh to be rendered
    void renderSingle(const Mesh* mesh);
    /// set per-mesh uniforms
    /// - Parameter mesh: mesh to take uniforms from
    void setMeshUniforms(const Mesh* mesh);
    
    /// submit mesh for one frame (mesh is rendered untransformed)
    /// - Parameter mesh: mesh
    void submitTempMesh(Mesh* mesh);
    /// clear temporary meshed (call at end of frame)
    void clearTempMesh();
    
    /// add persistent mesh
    /// - Parameter mesh: mesh
    void addMesh(Mesh* mesh);
    /// set the render mode (call before render)
    /// - Parameter mode: new render mode
    void setMode(RenderMode mode);
    /// set pointer to camera data
    /// - Parameters:
    ///   - viewPtr: pointer to view matrix data
    ///   - offPtr: pointer to offset vector data
    ///   - projPtr: pointer to projection matrix data
    void setPtr(const GLfloat *viewPtr, const GLfloat *offPtr, const GLfloat *projPtr);
    
    /// deprecated
    const std::vector<Mesh*>& getEditableMeshes();
    
    /// handle GUI for all shader controllers
    void handleGUI();
    
    
    ShaderController _vertecies, _edges, _faces, _slice;
    /*
    void submitHighlight(GLuint VAO, int index, RenderMode mode);
    void renderHighlight();
     */
private:
    RenderMode _mode = RenderMode::cells;
    
    const GLfloat* _viewPtr = nullptr;
    const GLfloat* _offPtr = nullptr;
    const GLfloat* _projPtr = nullptr;
    
    ShaderController* _active = nullptr;
    
    struct Settings {
        vec4
        vertexColor{1, 0, 0, 1},
        edgeColor{0, .6, .4, 1},
        faceColor{.9, .6, 1, 0.05},
        cellColor{.7, .6, .4, 1};
        /*
        vec4    vertexColorHighlight{1, 1, 0, 1},
                edgeColorHighlight{.5, .3, .1, 1},
                faceColorHighlight{.4, 1, .8, 0.15},
                cellColorHighlight{.4, .3, .7, 1};
         */
    };
    
    std::vector<Mesh*> _meshes;
    std::vector<Mesh*> _tempMeshes;
    
    int _enabled_count;
    
    uint32_t _meshDisableFlags = MeshFlags::empty;
    
    /*
    struct singleRender {
        singleRender(GLuint vao, int idx, RenderMode md)
                    : VAO(vao), index(idx), mode(md) {}
        GLuint VAO;
        int index;
        RenderMode mode;
    };
    
    std::vector<singleRender> _highlighted;
     */
};



// Experimental currently not implemented
struct FramebufferInitMode {
    int sharedDepthBufferIndex=-1;
};

struct RenderPassOperation {
    enum op {
        none,
        draw,
        clear,
        change_render_target
    } operation = none;
    union {
        vec4 clearColor;
        RenderMode drawMode;
        int renderTarget;
    } data;
};

class RenderPassController {
public:
    RenderPassController() = default;
    RenderPassController(const std::vector<RenderPassOperation>& operations,
                         const std::vector<FramebufferInitMode>& fb_inits);
    
    void execute(RenderPassOperation op);
    void draw(RenderMode mode);
    void resizeBuffers(int width, int height);
private:
    std::vector<RenderPassOperation> _operations {};
    std::vector<FramebufferHandler> _framebuffers {};
};





#endif /* renderer_hpp */
