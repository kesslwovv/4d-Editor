//
//  mesh.cpp
//  4d projection
//
//  Created by David on 22.10.22.
//

#include "mesh.hpp"

#include "renderer.hpp"


ColorInitData ColorInitData::Random()
{
    ColorInitData data;
    data.mode = ColorInitMode::random;
    return data;
}

ColorInitData ColorInitData::Constant(vec4 col)
{
    ColorInitData data;
    data.mode = ColorInitMode::constant;
    data.data.constColor = col;
    return data;
}

ColorInitData ColorInitData::Function(vec4 (*func)(vec4))
{
    ColorInitData data;
    data.mode = ColorInitMode::function;
    data.data.colorfunc = func;
    return data;
}


ColorInitData ColorInitData::TransformFunc(vec4 (*colorTransformFunc)(const vec4& localPos, const vec4& globalOffset, const mat4& modelMat))
{
    ColorInitData data;
    data.mode = ColorInitMode::transform_function;
    data.data.colorTransformFunc = colorTransformFunc;
    return data;
}

ColorInitData ColorInitData::File(const char *filename)
{
    ColorInitData data;
    data.mode = ColorInitMode::file;
    data.data.filename = filename;
    return data;
}

const float scaling = 3.33;
vec4 ColorInitData::getXYZ(vec4 pos)
{
    return {
        pos.x * scaling,
        pos.y * scaling,
        pos.z * scaling,
        1
    };
}

vec4 ColorInitData::getXZW(vec4 pos)
{
    return {
        pos.x * scaling,
        pos.z * scaling,
        pos.w * scaling,
        1
    };
}



void Mesh::loadGeometryASCII(const std::filesystem::path &absolute) {
    std::ifstream file(absolute.c_str());
    if (!file) {
        printf("file failed to open: %s\n", absolute.c_str());
        return;
    }
    
    std::string line;
    
    std::getline(file, line);
    _vertex_count = stoi(line);
    std::getline(file, line);
    _edge_count = stoi(line);
    std::getline(file, line);
    _triangle_count = stoi(line);
    std::getline(file, line);
    _tetrahedra_count = stoi(line);
    //    printf("%d\n%d\n%d\n%d\n", _vertex_count, _edge_count, _triangle_count, _tetrahedra_count);
    
    _vertecies.resize(_vertex_count);
    _edges.resize(_edge_count*2);
    _triangles.resize(_triangle_count*3);
    _tetrahedra.resize(_tetrahedra_count*4);
    
    std::string value;
    
    for (int i = 0; i < _vertex_count; i++) {
        std::getline(file, line);
        std::stringstream linestream(line);
        vec4 v(0);
        for (int j = 0; j < 4; j++) {
            std::getline(linestream, value, ',');
            v[j] = stof(value);
        }
        _vertecies[i] = v;
    }
    
    for (int i = 0; i < _edge_count; i++) {
        std::getline(file, line);
        std::stringstream linestream(line);
        for (int j = 0; j < 2; j++) {
            std::getline(linestream, value, ',');
            _edges[i*2+j] = stoi(value);
        }
    }
    
    for (int i = 0; i < _triangle_count; i++) {
        std::getline(file, line);
        std::stringstream linestream(line);
        for (int j = 0; j < 3; j++) {
            std::getline(linestream, value, ',');
            _triangles[i*3+j] = stoi(value);
        }
    }
    
    for (int i = 0; i < _tetrahedra_count; i++) {
        std::getline(file, line);
        std::stringstream linestream(line);
        for (int j = 0; j < 4; j++) {
            std::getline(linestream, value, ',');
            _tetrahedra[i*4+j] = stoi(value);
        }
    }
}

void Mesh::loadGeometryBIN(const std::filesystem::path &absolute) {
    std::ifstream file(absolute.c_str(), std::ios::binary);
    
    if (!file) {
        printf("file failed to open: %s\n", absolute.c_str());
        printf("errno: %d\n", errno);
        return;
    }
    
    assert(sizeof(size_t) == 8);
    
    file.read((char*)&_vertex_count, sizeof(size_t));
    file.read((char*)&_edge_count, sizeof(size_t));
    file.read((char*)&_triangle_count, sizeof(size_t));
    file.read((char*)&_tetrahedra_count, sizeof(size_t));

//    printf("vertex count: %zu\n", _vertex_count);
//    printf("edge count: %zu\n", _edge_count);
//    printf("triangle count: %zu\n", _triangle_count);
//    printf("tetrahedra count: %zu\n", _tetrahedra_count);

    _vertecies.resize(_vertex_count);
    _edges.resize(_edge_count*2);
    _triangles.resize(_triangle_count*3);
    _tetrahedra.resize(_tetrahedra_count*4);
    
    file.read((char*)_vertecies.data(), _vertex_count*BYTES_PER_VERTEX);
    file.read((char*)_edges.data(), _edge_count*BYTES_PER_EDGE);
    file.read((char*)_triangles.data(), _triangle_count*BYTES_PER_TRIANGLE);
    file.read((char*)_tetrahedra.data(), _tetrahedra_count*BYTES_PER_TETRAHEDRON);

}

Mesh::Mesh(path filename, FileFormat format, ColorInitData color_data, BuildTime buildSettings)
{
    path absolute = abspath(filename);
    
    if (format == FileFormat::automatic) {
        path ext = absolute.extension();
//        printf("ext: %s\n", ext.c_str());
        if (ext == ".bin") {
            format = FileFormat::binary;
        } else {
            format = FileFormat::ascii;
        }
    }
    
    switch (format) {
        case FileFormat::ascii:
            loadGeometryASCII(absolute);
            break;
        case FileFormat::binary:
            loadGeometryBIN(absolute);
            break;
            
        default:
            throw;
            break;
    }
    
//    initColorConstant({1, 0, 0, 1});
    reloadColors(color_data);
    
    if (buildSettings == BuildTime::immediate) {
        buildRenderObjects();
    }
}

Mesh::Mesh(size_t vertCnt, size_t edgeCnt, size_t faceCnt, size_t tetraCnt,
           vec4* verts, vec4* color, GLuint* edges, GLuint* faces, GLuint* tetra,
           BuildTime buildSettings)
:_vertex_count(vertCnt), _edge_count(edgeCnt), _triangle_count(faceCnt), _tetrahedra_count(tetraCnt)
{
    _colors.resize(_vertex_count);
    _vertecies.resize(_vertex_count);
    _edges.resize(_edge_count*2);
    _triangles.resize(_triangle_count*3);
    _tetrahedra.resize(_tetrahedra_count*4);
    
    if (color != nullptr) {
        memcpy(&_colors[0], color, _vertex_count*BYTES_PER_COLOR);
    }
    
    if (verts != nullptr) { memcpy(_vertecies.data(), verts, _vertex_count*BYTES_PER_VERTEX);}
    if (edges != nullptr) { memcpy(_edges.data(), edges, _edge_count*BYTES_PER_EDGE);}
    if (faces != nullptr) { memcpy(_triangles.data(), faces, _triangle_count*BYTES_PER_TRIANGLE);}
    if (tetra != nullptr) { memcpy(_tetrahedra.data(), tetra, _tetrahedra_count*BYTES_PER_TETRAHEDRON);}
    
    if (buildSettings == BuildTime::immediate) {
        buildRenderObjects();
    }
}

void Mesh::save(const char* filename)
{
    std::ofstream file(filename);
    
    file << _vertex_count << "\n";
    file << _edge_count << "\n";
    file << _triangle_count << "\n";
    file << _tetrahedra_count << "\n";
    
    for (int i = 0; i < _vertex_count; i++) {
        file << _vertecies[i].x << ","
             << _vertecies[i].y << ","
             << _vertecies[i].z << ","
             << _vertecies[i].w << "\n";
    }
    for (int i = 0; i < _edge_count; i++) {
        file << _edges[i*2]   << ","
             << _edges[i*2+1] << "\n";
    }
    
    for (int i = 0; i < _triangle_count; i++) {
        file << _triangles[i*3]   << ","
             << _triangles[i*3+1] << ","
             << _triangles[i*3+2] << "\n";
    }
    
    for (int i = 0; i < _tetrahedra_count; i++) {
        file << _tetrahedra[i*4]   << ","
             << _tetrahedra[i*4+1] << ","
             << _tetrahedra[i*4+2] << ","
             << _tetrahedra[i*4+3] << "\n";
    }
    
    file.close();
}


void Mesh::buildRenderObjects()
{
    // generate OpenGL objects
    glGenVertexArrays(1, &vertexVAO);
    glGenVertexArrays(1, &edgeVAO);
    glGenVertexArrays(1, &triangleVAO);
    glGenVertexArrays(1, &tetrahedraVAO);
    
    glGenBuffers(1, &edgeVEO);
    glGenBuffers(1, &triangleVEO);
    glGenBuffers(1, &tetrahedraVEO);
    
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &colorVBO);
    
    // init vertex/color buffers
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    START_TIMER("vbo buffer");
    glBufferData(GL_ARRAY_BUFFER, _vertex_count*BYTES_PER_VERTEX, _vertecies.data(), GL_STATIC_DRAW);
    END_TIMER("vbo buffer");

    glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
    START_TIMER("color buffer");
    glBufferData(GL_ARRAY_BUFFER, _vertex_count*BYTES_PER_COLOR, &_colors[0], GL_STATIC_DRAW);
    END_TIMER("color buffer");
    // vertex VAO
    glBindVertexArray(vertexVAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, BYTES_PER_VERTEX, (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, BYTES_PER_VERTEX, (void*)0);
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);

    
    // edge VAO
    glBindVertexArray(edgeVAO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, edgeVEO);
    START_TIMER("edge buffer");
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _edge_count * BYTES_PER_EDGE, _edges.data(), GL_STATIC_DRAW);
    END_TIMER("edge buffer");
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, BYTES_PER_VERTEX, (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, BYTES_PER_VERTEX, (void*)0);
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    
    glBindVertexArray(triangleVAO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangleVEO);
    START_TIMER("triangle buffer");
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _triangle_count * BYTES_PER_TRIANGLE, _triangles.data(), GL_STATIC_DRAW);
    END_TIMER("triangle buffer");
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, BYTES_PER_VERTEX, (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, BYTES_PER_VERTEX, (void*)0);
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    
    glBindVertexArray(tetrahedraVAO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tetrahedraVEO);
    START_TIMER("tetra buffer");
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _tetrahedra_count * BYTES_PER_TETRAHEDRON, _tetrahedra.data(), GL_STATIC_DRAW);
    END_TIMER("tetra buffer");
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, BYTES_PER_VERTEX, (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, BYTES_PER_VERTEX, (void*)0);
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

GLuint Mesh::getVAO(RenderMode mode) const
{
    switch (mode) {
        case RenderMode::vertecies:
            return vertexVAO;
        case RenderMode::edges:
            return edgeVAO;
        case RenderMode::faces:
            return triangleVAO;
        case RenderMode::cells:
            return tetrahedraVAO;
            
        default:
            throw;
            return 0;
    }
}

int Mesh::getDrawCount(RenderMode mode) const
{
    switch (mode) {
        case RenderMode::vertecies:
            return (int)_vertex_count;
        case RenderMode::edges:
            return (int)_edge_count*2;
        case RenderMode::faces:
            return (int)_triangle_count*3;
        case RenderMode::cells:
            return (int)_tetrahedra_count*4;
            
        default:
            exit(1);
            return 0;
    }
}

void Mesh::initColorConstant(vec4 constColor)
{
    _colors = std::vector<vec4>(_vertex_count);
    
    assert(_colors.size() == _vertex_count);
    for (vec4& color : _colors) {
        color = constColor;
    }
}

void Mesh::initColorRandom()
{
    _colors = std::vector<vec4>(_vertex_count);
//
    for (vec4& color : _colors) {
        color = randColor();
    }
//    for (int i = 0; i < _colors.size(); i++) {
//        _colors[i] = _vertecies[i] * 9.99f;
//        _colors[i].w = 1;
//    }
}

void Mesh::initColorFunc(vec4 (*colorfunc)(vec4))
{
    _colors = std::vector<vec4>(_vertex_count);
    
    for (int i = 0; i < _vertex_count; i++) {
        _colors[i] = colorfunc(_vertecies[i]);
    }
}

void Mesh::initColorTransformFunc(vec4 (*colorTransformFunc)(const vec4 &, const vec4 &, const mat4 &))
{
    _colors = std::vector<vec4>(_vertex_count);
    
    for (int i = 0; i < _vertex_count; i++) {
        _colors[i] = colorTransformFunc(_vertecies[i], _position, _transform);
    }
}

void Mesh::initColorFile(path filename)
{
    _colors = std::vector<vec4>(_vertex_count);
    
    std::ifstream file {abspath(filename)};
    
    size_t size;
    file.read((char*)&size, 8);
    
    if(size != _vertex_count) {
        printf("%s", filename.c_str());
        throw;
    }
    file.read((char*)&_colors[0], size*BYTES_PER_COLOR);
}

void Mesh::applyTransform(mat4 transform, vec4 offset)
{
    for (int i = 0; i < _vertex_count; i++) {
        _vertecies[i] = _vertecies[i] * transform;
        _vertecies[i] += offset;
    }
    reloadVBO();
}

void Mesh::applyOffset(vec4 offset)
{
    offsetVertecies(offset);
    reloadVBO();
}

void Mesh::offsetVertecies(vec4 offset)
{
    for (int i = 0; i < _vertex_count; i++) {
        _vertecies[i] += offset;
    }
}

void Mesh::reloadVBO()
{
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    if (_vertecies.size() == _vertex_count) {
        glBufferSubData(GL_ARRAY_BUFFER, 0, _vertex_count*BYTES_PER_VERTEX, (void*)_vertecies.data());
    } else {
        _vertex_count = _vertecies.size();
        glBufferData(GL_ARRAY_BUFFER, _vertex_count*BYTES_PER_VERTEX, (void*)_vertecies.data(), GL_STATIC_DRAW);

    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mesh::reloadEdges()
{
    //    return;
    glBindVertexArray(edgeVAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, edgeVEO);
    if (_edges.size() == _edge_count * 2) {
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, _edge_count*BYTES_PER_EDGE, (void*)_edges.data());
    } else {
        _edge_count = _edges.size() / 2;
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, _edge_count*BYTES_PER_EDGE, (void*)_edges.data(), GL_STATIC_DRAW);
    }
    checkError();
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    checkError();
}

void Mesh::reloadFaces()
{
    glBindVertexArray(triangleVAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangleVEO);
    if (_triangles.size() == _triangle_count * 3) {
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, _triangle_count*BYTES_PER_TRIANGLE, (void*)_triangles.data());
    } else {
        _triangle_count = _triangles.size() / 3;
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, _triangle_count*BYTES_PER_TRIANGLE, (void*)_triangles.data(), GL_STATIC_DRAW);
    }
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


void Mesh::reloadCells()
{
    glBindVertexArray(tetrahedraVAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tetrahedraVEO);
    if (_tetrahedra.size() == _tetrahedra_count * 4) {
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, _tetrahedra_count*BYTES_PER_TETRAHEDRON, (void*)_tetrahedra.data());
    } else {
        _tetrahedra_count = _tetrahedra.size() / 4;
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, _tetrahedra_count*BYTES_PER_TETRAHEDRON, (void*)_tetrahedra.data(), GL_STATIC_DRAW);
    }
    checkError();
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    checkError();
}

void Mesh::reloadColors(ColorInitData color_data)
{
    switch (color_data.mode) {
        case ColorInitMode::random:
            initColorRandom();
            break;
        case ColorInitMode::function:
            initColorFunc(color_data.data.colorfunc);
            break;
        case ColorInitMode::constant:
            initColorConstant(color_data.data.constColor);
            break;
            
        case ColorInitMode::transform_function:
            initColorTransformFunc(color_data.data.colorTransformFunc);
            break;
        case ColorInitMode::file:
            initColorFile(color_data.data.filename);
            break;
            
        default:
            throw ;
            break;
    }
}

void Mesh::reloadColorBuffer()
{
    glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
    
    glBufferData(GL_ARRAY_BUFFER, _colors.size()*BYTES_PER_COLOR, (void*)&_colors[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

size_t Mesh::getVertexCount()
{
    return _vertex_count;
}

size_t Mesh::getEdgeCount()
{
    return _edge_count;
}

size_t Mesh::getFaceCount()
{
    return _triangle_count;
}

size_t Mesh::getCellCount()
{
    return _tetrahedra_count;
}

const std::vector<vec4>& Mesh::getVertecies() {
    return _vertecies;
}

void Mesh::setPos(glm::vec4 pos) {
    _position = pos;
}

const GLfloat* Mesh::getPos() const
{
    return value_ptr(_position);
}

const GLfloat* Mesh::getTransform() const
{
    return value_ptr(_transform);
}


void Mesh::reloadBuffers()
{
    reloadVBO();
    reloadEdges();
    reloadFaces();
    reloadCells();
    reloadColorBuffer();
}

void Mesh::set_flags(uint32_t flags)
{
    _flags = flags;
}

void Mesh::add_flags(uint32_t flags)
{
    _flags |= flags;
}

void Mesh::rem_flags(uint32_t flags)
{
    _flags &= ~flags;
}


mat4 rotateAxis(int a, int b, float angle)
{
    float s=sin(angle), c=cos(angle);
    mat4 rot = mat4(1);
    rot[a][a] = c;
    rot[a][b] = s;
    rot[b][a] = -s;
    rot[b][b] = c;
    return rot;
}

MeshTransformHandler::MeshTransformHandler(Mesh* mesh)
: _mesh(mesh)
{
    
}

void MeshTransformHandler::handleGUI()
{
    ImGui::DragFloat4("position", &_mesh->_position.x, 0.3);
    
    ImGui::SliderFloat("size", &_size, 0, 10);
    ImGui::SliderFloat4("axis scale", &_scale_axis[0], -1, 1);

    ImGui::SliderFloat3("XY XZ YZ rotation", &_angles[0], -M_PI, M_PI);
    ImGui::SliderFloat3("ZW YW XW rotation", &_angles[3], -M_PI, M_PI);
    
    if (ImGui::TreeNode("Animation")) {
        
        if (_isAnimating) {
            if (!_paused) {
                _timePassed += _animationSpeed;
            }
            if (ImGui::Button("Stop Animation")) {
                _isAnimating = false;
                _timePassed = 0;
                _paused = false;
            }
            ImGui::SameLine();
            ImGui::Text("t: %f", _timePassed);
            
            if (_paused) {
                if (ImGui::Button("Continue")) {
                    _paused = false;
                }
            } else {
                if (ImGui::Button("Pause")) {
                    _paused = true;
                }
            }
            
        } else {
            if (ImGui::Button("Start Animation")) {
                _isAnimating = true;
            }
            ImGui::BeginDisabled();
            ImGui::Button("Pause");
            ImGui::EndDisabled();
        }
        
        ImGui::SliderFloat("animation Speed", &_animationSpeed, 0, 10, "%.5f", ImGuiSliderFlags_Logarithmic);
        
        ImGui::SliderFloat3("XY XZ YZ animation", &_rotSpeed[0], -M_PI, M_PI);
        ImGui::SliderFloat3("ZW YW XW animation", &_rotSpeed[3], -M_PI, M_PI);
        
        ImGui::TreePop();
    }
    
    updateTransform();
}

void MeshTransformHandler::updateTransform()
{
    mat4 rotation = mat4(_size);
    
    rotation[0][0] *= _scale_axis[0];
    rotation[1][1] *= _scale_axis[1];
    rotation[2][2] *= _scale_axis[2];
    rotation[3][3] *= _scale_axis[3];
    
    rotation *= rotateAxis(0, 1, getAngle(0));
    rotation *= rotateAxis(0, 2, getAngle(1));
    rotation *= rotateAxis(1, 2, getAngle(2));
    rotation *= rotateAxis(2, 3, getAngle(3));
    rotation *= rotateAxis(1, 3, getAngle(4));
    rotation *= rotateAxis(0, 3, getAngle(5));
    _mesh->_transform = rotation;
}

float MeshTransformHandler::getAngle(int i)
{
    return _angles[i] + _timePassed * _rotSpeed[i];
}



template<size_t size, typename T>
bool contains(T* arr, T compare)
{
    for (int i = 0; i < size; i++) {
        if (arr[i] == compare) {
            return true;
        }
    }
    return false;
}

template<size_t size, typename T>
bool equal(T* arr, T* compare)
{
    for (int i = 0; i < size; i++) {
        if (arr[i] != compare[i]) {
            return false;
        }
    }
    return true;
}

template<size_t size, typename T>
void replace(T* arr, T orig, T rep)
{
    for (int i = 0; i < size; i++) {
        if (arr[i] == orig) {
            arr[i] = rep;
        }
    }
}

template<size_t size, typename T>
bool isPerm(T* arr, T* compare)
{
    bool flagsA[size];
    bool flagsB[size];
    for (int i = 0; i < size; i++) {
        flagsA[i] = false;
        flagsB[i] = false;
    }
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (arr[i] == compare[j]) {
                flagsA[i] = true;
                flagsB[j] = true;
                break;
            }
        }
    }
    for (int i = 0; i < size; i++) {
        if (!flagsA[i]) {
            return false;
        }
        if (!flagsB[i]) {
            return false;
        }
    }
    return true;
}
