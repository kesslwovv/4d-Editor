//
//  mesh.hpp
//  4d projection
//
//  Created by David on 22.10.22.
//

#ifndef mesh_hpp
#define mesh_hpp

#include "glm/glm.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "texture.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <tuple>

#define BYTES_PER_VERTEX 16
#define BYTES_PER_COLOR 16
#define BYTES_PER_EDGE 8
#define BYTES_PER_TRIANGLE 12
#define BYTES_PER_TETRAHEDRON 16

using namespace glm;
using std::get;
using std::filesystem::path;

template<typename T>
using tup2 = std::tuple<T, T>;
template<typename T>
using tup3 = std::tuple<T, T, T>;
template<typename T>
using tup4 = std::tuple<T, T, T, T>;

/// Render modes
enum class RenderMode
{
    vertecies = 1,
    edges = 2,
    faces = 3,
    cells = 4,
    disable = 5
};

// TODO: define bitwise operation for MeshFlags so I can make it `enum class`
/// Flags to be used by meshes
struct MeshFlags
{
    enum : uint32_t {
        empty        = 0,
        disable      = 1 << 0,
        disableVert  = 1 << 1,
        disableEdge  = 1 << 2,
        disableFace  = 1 << 3,
        disableTetra = 1 << 4,
    };
};



/// File format of mesh
enum class FileFormat {
    ascii,
    binary,
    automatic
};

enum class BuildTime {
    immediate,
    delayed
};

enum class ColorInitMode {
    random,
    constant,
    function,
    transform_function,
    file,
};

struct ColorInitData {
    ColorInitMode mode;
    union ColorData {
        vec4 constColor;
        vec4 (*colorfunc)(vec4);
        vec4 (*colorTransformFunc)(const vec4& localPos, const vec4& globalOffset, const mat4& modelMat);
        const char* filename;
    } data;
    
    static ColorInitData Random();
    static ColorInitData Constant(vec4 col);
    static ColorInitData Function(vec4 (*func)(vec4));
    static ColorInitData TransformFunc(vec4 (*colorTransformFunc)(const vec4& localPos, const vec4& globalOffset, const mat4& modelMat));
    static ColorInitData File(const char* filename);
    
    // static functions
    static vec4 getXYZ(vec4 pos);
    static vec4 getXZW(vec4 pos);
};

class Mesh
{
public:
    Mesh(path filename, FileFormat format=FileFormat::automatic,
         ColorInitData color_data=ColorInitData::Function(ColorInitData::getXZW), BuildTime buildSettings=BuildTime::immediate);
    Mesh(size_t vertCnt, size_t edgeCnt, size_t faceCnt, size_t tetraCnt,
         vec4* vert, vec4* color, GLuint* edges, GLuint* faces, GLuint* tetra,
         BuildTime buildSettings=BuildTime::immediate);

    Mesh() = default;
    void save(const char* filename);
    
    GLuint getVAO(RenderMode mode) const;
    int getDrawCount(RenderMode mode) const;
    
    const GLfloat* getPos() const;
    const GLfloat* getTransform() const;
    void setPos(vec4 pos);
    void reloadColors(ColorInitData color_data);
    
    void applyTransform(mat4 transform, vec4 offset);
    void applyOffset(vec4 offset);
    void offsetVertecies(vec4 offset);
    void reloadVBO();
    void reloadEdges();
    void reloadFaces();
    void reloadCells();
    void reloadColorBuffer();
    void reloadBuffers();
    
    size_t getVertexCount();
    size_t getEdgeCount();
    size_t getFaceCount();
    size_t getCellCount();
    
    

    const std::vector<vec4>& getVertecies();
    
    void initColor(ColorInitData color_data);
    void initColorConstant(vec4 color);
    void initColorRandom();
    void initColorFunc(vec4 (*colorfunc)(vec4 pos));
    void initColorTransformFunc(vec4 (*colorTransformFunc)(const vec4& localPos, const vec4& globalOffset, const mat4& modelMat));
    void initColorFile(path filename);
    
    void loadGeometryASCII(const std::filesystem::path &absolute);
    void loadGeometryBIN(const std::filesystem::path &absolute);

    void set_flags(uint32_t flags);
    
    void add_flags(uint32_t flags);
    void rem_flags(uint32_t flags);

    // TODO: make getter/setter methods
    std::vector<vec4> _vertecies;
    std::vector<vec4> _colors {};
    std::vector<GLuint> _edges;
    std::vector<GLuint> _triangles;
    std::vector<GLuint> _tetrahedra;
    
    vec4 _position = vec4(0);
    mat4 _transform = mat4(1);
    
    void buildRenderObjects();
    
    uint32_t _flags = MeshFlags::empty;
    
    Texture* _tex = nullptr;
private:
    struct Tetrahedron {
        Tetrahedron(float _a, float _b, float _c, float _d) :
        a(_a), b(_b), c(_c), d(_d) {};
        float a, b, c, d;
    };
    
    GLuint VBO, colorVBO;
    GLuint vertexVAO;
    GLuint edgeVAO, edgeVEO;
    GLuint triangleVAO, triangleVEO;
    GLuint tetrahedraVAO, tetrahedraVEO;
    
    size_t _vertex_count = 0, _edge_count = 0, _triangle_count = 0, _tetrahedra_count = 0;
};

mat4 rotateAxis(int a, int b, float angle);

/// a handler for mesh rotation
class MeshTransformHandler {
public:
    MeshTransformHandler() = default;
    MeshTransformHandler(Mesh* mesh);
    
    void handleGUI();
    void updateTransform();
private:
    float getAngle(int i);
    
    Mesh* _mesh;
    float _size{1};
    float _angles[6] {0};
    
    float _scale_axis[4] {1, 1, 1, 1};
    
    float _rotSpeed[6] {0};
    bool _isAnimating = false;
    float _animationSpeed = 0.01;
    float _timePassed = 0;
    bool _paused = false;
};

template<size_t size, typename T>
bool contains(T* arr, T compare);
template<size_t size, typename T>
bool equal(T* arr, T* compare);
template<size_t size, typename T>
void replace(T* arr, T orig, T rep);
template<size_t size, typename T>
bool isPerm(T* arr, T* compare);


#endif /* mesh_hpp */
