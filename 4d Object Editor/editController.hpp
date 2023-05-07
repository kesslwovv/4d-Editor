//
//  editController.hpp
//  4d Object Editor
//
//  Created by David on 09.11.22.
//

#ifndef editController_hpp
#define editController_hpp


#include <tuple>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "imgui/imgui.h"

#include "FileBrowser/ImGuiFileBrowser.h"

#include "mesh.hpp"
#include "camera.hpp"
#include "renderer.hpp"
#include "utils.hpp"

using namespace glm;
using std::get;

enum EditMode
{
    noAction = 0,
    dragVertex = 1,
    editTetrahedron = 2,
    addTetrahedron = 3,
    swapVertecies = 4,
};


class EditController {
public:
    EditController() = default;
    EditController(Camera* cam, Mesh* mesh, RenderController* renderer);
    
    void handleInput(GLFWwindow* window);
    
    void handleGUI();
private:
    constexpr static const char* const _openPopupName = "open mesh";
    constexpr static const char* const _savePopupName = "save mesh";
    
    constexpr static const char* const _openTexPopupName = "open texture";
    constexpr static const char* const _openColPopupName = "open colordata";

    EditMode _mode = EditMode::noAction;
    
    Camera* _cam;
    Mesh* _mesh;
    RenderController* _renderer;
    
    MeshTransformHandler _transformHandler;
    
    imgui_addons::ImGuiFileBrowser _modelBrowser{};
    imgui_addons::ImGuiFileBrowser _textureBrowser{};
    imgui_addons::ImGuiFileBrowser _colordataBrowser{};
    
    std::string _file = "";
    
    struct Input {
        bool mousePressed = false;
        bool mouseLastFrame = false;
        bool mousePressedRight = false;
        bool keyControl = false;
        bool keyAlt = false;
        bool keyComma = false;
        bool keyPeriod = false;
        bool keySlash = false;
    } _input;
    
    // main handler methods
    void handleHistory(GLFWwindow *window);
    void updateInput(GLFWwindow *window);
    
    void handleNoAction();
    void handleInsertTetrahedron();
    void handleModeAddTetrahedron();
    void handleModeSwapVertecies();
    
    
    struct EditHistory {
        std::vector<Mesh> editHistory {};
        size_t currentIndex = 0;
        bool canTraverse = true;
    } _history;
    
    void undo();
    void redo();
    void logMesh();
    void clearHistory();
    
    
    struct VertexDragInfo {
        int index = 0;
        float depth = 0;
        float perp = 0;
    } _vertexDrag;
    
    struct TetrahedronEditInfo {
        bool canEditTetrahedron = true;
        bool canAddTetrahedronVertex = true;
        int tetrahedronIndecies[4]{0};
        int tetrahedronPointsAdded{0};
    } _tetrahedronEdit;
    
    struct TetrahedronSwapInfo {
        int swappableTetrahedra[3];
        int swappableTetrahedronPoints[5];
        // shared, shared, shared, unique1, unique2
        bool canSwapTetrahedron = true;
        bool canAddSwappableTetrahedron = true;
        int tetrahedraAdded = 0;
    } _tetrahedronSwap;
    
    
    
    bool _canRemoveVertex = true;
    
    float _dragSpeed = 0.015;
    
    void getVertexUnderMouse(float &closestDist, glm::vec4 &closestVertex, int& closestVertexIndex);
    int getTetrahedronUnderMouse();
    
    void handleGetVertex();
    
    void handleHoverVertex();
    void handleHoverTetrahedron();
    
    void highlightActiveVertex();
    void highlightSelectedVertecies();
    
    void handleDragVertex(GLFWwindow* window);
    void handleRemoveVertex();
    
    void handleEditTetrahedron();
    void handleAddPointToTetrahedron();
    
    void handleFileDialog();

    void saveMesh();
    
    void handleAddSwapableTetrahedron();
    void handleSwapTetrahedron1();
    void handleSwapTetrahedron2();
    
    void handleSwapTetrahedronHighlight();
};

#endif /* editController_hpp */
