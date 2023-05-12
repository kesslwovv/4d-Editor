//
//  editController.cpp
//  4d Object Editor
//
//  Created by David on 09.11.22.
//

#include "editController.hpp"

float vertexSpawnDepth = 2;

EditController::EditController(Camera* cam, Mesh* mesh, RenderController* renderer)
:
_cam(cam),
_mesh(mesh),
_renderer(renderer),
_transformHandler(mesh)
{
    _modelBrowser.current_path = PATH_REC;
    _textureBrowser.current_path = PATH_REC;
    _colordataBrowser.current_path = PATH_REC;
}

void EditController::handleInput(GLFWwindow *window)
{
    updateInput(window);
    
    handleHistory(window);
    
    /*
    switch (_mode) {
        case EditMode::noAction:
            handleNoAction();
            break;
        case EditMode::dragVertex:
            if (!_input.mousePressed) {
                _mode = EditMode::noAction;
            } else {
                handleDragVertex(window);
            }
            break;
        case EditMode::editTetrahedron:
            if (!_input.keyControl) {
                _mode = EditMode::noAction;
            } else {
                handleInsertTetrahedron();
            }
            break;
        case EditMode::addTetrahedron:
            if (!_input.keyAlt) {
                _mode = EditMode::noAction;
            } else {
                handleModeAddTetrahedron();
            }
            break;
        case EditMode::swapVertecies:
            if (!_input.keyComma) {
                _tetrahedronSwap.tetrahedraAdded = 0;
                _mode = EditMode::noAction;
            } else {
                handleModeSwapVertecies();
            }
            break;
        default:
            throw;
            break;
    }
     */
}

void EditController::handleGUI()
{
    ImGui::Begin("editor info");
    
    if (ImGui::Button("save mesh")) {
        if (_file.empty()) {
            ImGui::OpenPopup(_openPopupName);
        } else {
            saveMesh();
        }
        
    }
    
    if (ImGui::Button("save mesh as")) {
//        _saveMode = SaveMode::save;
        ImGui::OpenPopup(_savePopupName);
    }
    
    if (ImGui::Button("open mesh")) {
//        _saveMode = SaveMode::open;
        ImGui::OpenPopup(_openPopupName);
    }
    
    if (ImGui::Button("select texture")) {
        ImGui::OpenPopup(_openTexPopupName);
    }
    
    if (ImGui::Button("select colordata")) {
        ImGui::OpenPopup(_openColPopupName);
    }
    
    if (ImGui::Button("clear texture")) {
        _mesh->_tex = nullptr;
    }
    
    handleFileDialog();
    
    _transformHandler.handleGUI();
    
    if (ImGui::Button("cull duplicate tetrahedra")) {
        throw;
//        _mesh->cullDuplicateTetrahedra();
    }
    
    if (ImGui::Button("reload faces/edges from tetrahedra")) {
        throw;
//        _mesh->rebuildConnectionsFromTetrahedra();
    }
    
    switch (_mode) {
        case EditMode::addTetrahedron:
            for (int i = 0; i < _tetrahedronEdit.tetrahedronPointsAdded; i++) {
                ImGui::PushID(i);
                ImGui::Text("%d : %d", i, _tetrahedronEdit.tetrahedronIndecies[i]);
                ImGui::PopID();
            }
            break;
            
        case EditMode::editTetrahedron:
            ImGui::BeginDisabled();
            ImGui::Checkbox("can edit tetrahedron", &_tetrahedronEdit.canEditTetrahedron);
            ImGui::Text("Vertex: %d", _vertexDrag.index);
            ImGui::EndDisabled();
            break;
            
        case EditMode::swapVertecies:
            ImGui::Text("swapping\n num: %d", _tetrahedronSwap.tetrahedraAdded);
            break;
            
        default:
            break;
    }
    ImGui::Text("Cell count: %zu", _mesh->getCellCount());
//    for (int i = 0; i < _mesh->getCellCount(); i++) {
//        tup4<int> tetra = _mesh->getTetrahedronIndices(i);
//        ImGui::PushID(i);
//        ImGui::Text("Tetrahedron: %d, %d, %d, %d",
//                    get<0>(tetra),
//                    get<1>(tetra),
//                    get<2>(tetra),
//                    get<3>(tetra));
//        ImGui::PopID();
//    }
    
    ImGui::End();
}


void EditController::handleHistory(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_N)) {
        if (_history.canTraverse) {
            _history.canTraverse = false;
            undo();
        }
    } else if (glfwGetKey(window, GLFW_KEY_M)) {
        if (_history.canTraverse) {
            _history.canTraverse = false;
            redo();
        }
    } else {
        _history.canTraverse = true;
    }
}

void EditController::updateInput(GLFWwindow *window) {
    _input.mouseLastFrame = _input.mousePressed;
    _input.mousePressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    _input.mousePressedRight = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
    _input.keyControl = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL)
    || glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL);
    _input.keyAlt = glfwGetKey(window, GLFW_KEY_LEFT_ALT)
    || glfwGetKey(window, GLFW_KEY_RIGHT_ALT);
    _input.keyComma = glfwGetKey(window, GLFW_KEY_COMMA);
    _input.keyPeriod = glfwGetKey(window, GLFW_KEY_PERIOD);
    _input.keySlash = glfwGetKey(window, GLFW_KEY_SLASH);
}

/*
void EditController::handleNoAction() {
    if (_input.keyControl) {
        logMesh();
        _mode = EditMode::editTetrahedron;
    } else if (_input.keyAlt) {
        logMesh();
        _mode = EditMode::addTetrahedron;
    }
    else if (_input.keyComma && _input.mousePressed) {
        logMesh();
        handleGetVertex();
        _mode = EditMode::dragVertex;
    } else if (_input.mousePressedRight) {
        if (_canRemoveVertex) {
            logMesh();
            handleRemoveVertex();
        }
    }
    else if (_input.keyComma) {
        logMesh();
        _mode = EditMode::swapVertecies;
    } else {
//        handleHoverVertex();
    }
    _canRemoveVertex = !_input.mousePressedRight;
}

void EditController::handleInsertTetrahedron() {
    if (_input.mousePressed) {
        if (_tetrahedronEdit.canEditTetrahedron) {
            _tetrahedronEdit.canEditTetrahedron = false;
            handleEditTetrahedron();
        }
    } else {
        handleHoverTetrahedron();
        highlightActiveVertex();
        _tetrahedronEdit.canEditTetrahedron = true;
    }
}

void EditController::handleModeAddTetrahedron() {
    if (_input.mousePressed) {
        if (_tetrahedronEdit.canEditTetrahedron) {
            _tetrahedronEdit.canEditTetrahedron = false;
            handleAddPointToTetrahedron();
        }
    } else {
        handleHoverVertex();
        highlightSelectedVertecies();
        _tetrahedronEdit.canEditTetrahedron = true;
    }
}

void EditController::handleModeSwapVertecies() {
    if (_input.mousePressed) {
        if (_tetrahedronSwap.canAddSwappableTetrahedron) {
            _tetrahedronSwap.canAddSwappableTetrahedron = false;
            handleAddSwapableTetrahedron();
        }
    } else {
        _tetrahedronSwap.canAddSwappableTetrahedron = true;
    }
    if (_input.keyPeriod) {
        if (_tetrahedronSwap.canSwapTetrahedron) {
            _tetrahedronSwap.canSwapTetrahedron = false;
            handleSwapTetrahedron1();
        }
    } else if (_input.keySlash) {
        if (_tetrahedronSwap.canSwapTetrahedron) {
            _tetrahedronSwap.canSwapTetrahedron = false;
            handleSwapTetrahedron2();
        }
    } else {
        _tetrahedronSwap.canSwapTetrahedron = true;
    }
    handleSwapTetrahedronHighlight();
}

*/
void EditController::getVertexUnderMouse(float &closestDist, glm::vec4 &closestVertex, int& closestVertexIndex) {
    closestDist = FLT_MAX;
    closestVertexIndex = -1;
    
    for (int i = 0; i < _mesh->getVertecies().size(); i++) {
        vec4 v = _mesh->getVertecies()[i];
        float dist = raySphereIntersect(v, .2,
                                        _cam->getPos(),
                                        _cam->getMouseRayDir(),
                                        _cam->getPerp());
        if (dist > 0 && dist < closestDist) {
            closestDist = dist;
            closestVertex = v;
            closestVertexIndex = i;
        }
    }
}

/*
void EditController::handleGetVertex()
{
    float closestDist;
    glm::vec4 closestVertex;
    int closestVertexIndex = -1;
    getVertexUnderMouse(closestDist, closestVertex, closestVertexIndex);
    
    if (closestDist != FLT_MAX) {
        _vertexDrag.index = closestVertexIndex;
        _vertexDrag.depth = closestDist;
        _vertexDrag.perp = dot(closestVertex, _cam->getPerp());
    } else {
        closestVertex = _cam->getPos() + _cam->getMouseRayDir() * vertexSpawnDepth;
        _vertexDrag.index = _mesh->addVertex(closestVertex);
        _vertexDrag.depth = vertexSpawnDepth;
        _vertexDrag.perp = 0;
    }
}

void EditController::highlightActiveVertex() {
//    _renderer->submitHighlight(_mesh->getVAO(RenderMode::vertecies), _vertexDrag.index, RenderMode::vertecies);
}

void EditController::handleDragVertex(GLFWwindow* window)
{
#define ADD_VAR_ON_KEY(var, key) {if(glfwGetKey(window, key)) var += _dragSpeed;}
#define SUB_VAR_ON_KEY(var, key) {if(glfwGetKey(window, key)) var -= _dragSpeed;}
    ADD_VAR_ON_KEY(_vertexDrag.depth, GLFW_KEY_Z);
    SUB_VAR_ON_KEY(_vertexDrag.depth, GLFW_KEY_X);
    ADD_VAR_ON_KEY(_vertexDrag.perp, GLFW_KEY_C);
    SUB_VAR_ON_KEY(_vertexDrag.perp, GLFW_KEY_V);
#undef ADD_VAR_ON_KEY
#undef SUB_VAR_ON_KEY
    
    vec4 newVertexPos = _cam->getPos()
                    + _cam->getMouseRayDir() * _vertexDrag.depth
                    + _cam->getPerp() * _vertexDrag.perp;
    _mesh->editVertex(_vertexDrag.index, newVertexPos);
    
    highlightActiveVertex();
}

void EditController::handleRemoveVertex()
{
    float closestDist;
    glm::vec4 closestVertex;
    int closestVertexIndex = -1;
    getVertexUnderMouse(closestDist, closestVertex, closestVertexIndex);
    
    if (closestVertexIndex != -1) {
        _mesh->removeVertex(closestVertexIndex);
    }
}

int EditController::getTetrahedronUnderMouse() {
    int closestCell = -1;
    float closest = FLT_MAX;
    for (int i = 0; i < _mesh->getCellCount(); i++) {
        auto tetra = _mesh->getTetrahedron(i);
        float dist = raySimplexIntersect(std::get<0>(tetra),
                                         std::get<1>(tetra),
                                         std::get<2>(tetra),
                                         std::get<3>(tetra),
                                         _cam->getPos(),
                                         _cam->getMouseRayDir());
        if (dist > 0 && dist < closest) {
            closest = dist;
            closestCell = i;
        }
    }
    return closestCell;
}

void EditController::handleEditTetrahedron()
{
    int closestCell = getTetrahedronUnderMouse();
    if (closestCell != -1) {
        _mesh->insertPoint(_vertexDrag.index, closestCell);
    } else {
        _tetrahedronEdit.canEditTetrahedron = true;
    }
}

void EditController::handleAddPointToTetrahedron()
{
    float closestDist;
    glm::vec4 closestVertex;
    int closestVertexIndex = -1;
    getVertexUnderMouse(closestDist, closestVertex, closestVertexIndex);
 
    if (closestVertexIndex == -1) {
        _tetrahedronEdit.canAddTetrahedronVertex = true;
        return;
    }
    for (int i = 0; i < _tetrahedronEdit.tetrahedronPointsAdded; i++) {
        if (_tetrahedronEdit.tetrahedronIndecies[i] == closestVertexIndex) {
            _tetrahedronEdit.canAddTetrahedronVertex = true;
            return;
        }
    }
    _tetrahedronEdit.tetrahedronIndecies[_tetrahedronEdit.tetrahedronPointsAdded] = closestVertexIndex;
    _tetrahedronEdit.tetrahedronPointsAdded++;
    
    if (_tetrahedronEdit.tetrahedronPointsAdded == 4) {
        _mesh->addTetrahedralConnections(_tetrahedronEdit.tetrahedronIndecies[0],
                              _tetrahedronEdit.tetrahedronIndecies[1],
                              _tetrahedronEdit.tetrahedronIndecies[2],
                              _tetrahedronEdit.tetrahedronIndecies[3]);
        _tetrahedronEdit.tetrahedronPointsAdded = 0;
    }
}
 */

void EditController::handleFileDialog()
{
    if ( _modelBrowser.showFileDialog(_openPopupName,
                                      imgui_addons::ImGuiFileBrowser::DialogMode::OPEN,
                                      {0, 0}, ".obj4d,.obj4d.bin,.bin")) {
        _file = _modelBrowser.selected_path;
        Texture* oldMeshTex = _mesh->_tex;
        *_mesh = Mesh(_file.c_str());
        _mesh->_tex = oldMeshTex;
    }
    
    if ( _modelBrowser.showFileDialog(_savePopupName,
                                      imgui_addons::ImGuiFileBrowser::DialogMode::SAVE,
                                      {0, 0}, ".obj4d,.obj4d.bin,.bin")) {
        _file = _modelBrowser.selected_path;
        _mesh->save(_file.c_str());
    }
    
    if (_textureBrowser.showFileDialog(_openTexPopupName,
                                       imgui_addons::ImGuiFileBrowser::DialogMode::OPEN,
                                       {0, 0}, ".texture")) {
        _mesh->_tex = new Texture(_textureBrowser.selected_path);
    }
    
    if (_colordataBrowser.showFileDialog(_openColPopupName,
                                       imgui_addons::ImGuiFileBrowser::DialogMode::OPEN,
                                       {0, 0}, ".colordata,.colordata.bin,*.*")) {
        _mesh->reloadColors(ColorInitData::File(_colordataBrowser.selected_path.c_str()));
        _mesh->reloadColorBuffer();
    }

}

void EditController::saveMesh()
{
    _mesh->save(_file.c_str());
}

/*
void EditController::handleHoverVertex()
{
    float closestDist;
    glm::vec4 closestVertex;
    int closestVertexIndex = -1;
    getVertexUnderMouse(closestDist, closestVertex, closestVertexIndex);
    
    if (closestVertexIndex != -1) {
//        _renderer->submitHighlight(_mesh->getVAO(RenderMode::vertecies), closestVertexIndex, RenderMode::vertecies);
    }
}

void EditController::handleHoverTetrahedron()
{
    int cell = getTetrahedronUnderMouse();
    
    if (cell != -1) {
//        _renderer->submitHighlight(_mesh->getVAO(RenderMode::cells), cell, RenderMode::cells);
    }
}

void EditController::highlightSelectedVertecies()
{
    GLuint VAO = _mesh->getVAO(RenderMode::vertecies);
    for (int i = 0; i < _tetrahedronEdit.tetrahedronPointsAdded; i++) {
//        _renderer->submitHighlight(VAO, _tetrahedronEdit.tetrahedronIndecies[i], RenderMode::vertecies);
    }
}

void EditController::handleAddSwapableTetrahedron()
{
    int tetrahedron = getTetrahedronUnderMouse();
    if (tetrahedron == -1) {
        return;
    }
    if (_tetrahedronSwap.tetrahedraAdded == 2) {
        _tetrahedronSwap.swappableTetrahedra[0] = _tetrahedronSwap.swappableTetrahedra[1];
        _tetrahedronSwap.swappableTetrahedra[1] = tetrahedron;
        
    } else if (_tetrahedronSwap.tetrahedraAdded == 3) {
        
    } else if (_tetrahedronSwap.tetrahedraAdded == 0) {
        _tetrahedronSwap.swappableTetrahedra[0] = tetrahedron;
        tup4<int> points = _mesh->getTetrahedronIndices(tetrahedron);
        _tetrahedronSwap.swappableTetrahedronPoints[0] = get<0>(points);
        _tetrahedronSwap.swappableTetrahedronPoints[1] = get<1>(points);
        _tetrahedronSwap.swappableTetrahedronPoints[2] = get<2>(points);
        _tetrahedronSwap.swappableTetrahedronPoints[3] = get<3>(points);
        _tetrahedronSwap.tetrahedraAdded++;
        
    } else if (_tetrahedronSwap.tetrahedraAdded == 1) {
        int proposedNewPoints[5];
        int sharedPoints = 0;
        
        tup4<int> pointTuple = _mesh->getTetrahedronIndices(tetrahedron);
        int points[4] {get<0>(pointTuple), get<1>(pointTuple), get<2>(pointTuple), get<3>(pointTuple)};
        
        for (int i = 0; i < 4; i++) {
            int comparer = _tetrahedronSwap.swappableTetrahedronPoints[i];
            for (int j = 0; j < 4; j++) {
                if (comparer == points[j]) {
                    proposedNewPoints[sharedPoints] = comparer;
                    sharedPoints++;
                    points[j] = -1;
                    goto isEqual;
                }
            }
            proposedNewPoints[3] = comparer;
            
            isEqual:
            continue;
        }
        if (sharedPoints != 3) {
            return;
        }
        
        for (int i = 0; i < 4; i++) {
            if (points[i] != -1) {
                proposedNewPoints[4] = points[i];
            }
        }
        for (int i = 0; i < 5; i++) {
            _tetrahedronSwap.swappableTetrahedronPoints[i] = proposedNewPoints[i];
        }
        
        _tetrahedronSwap.swappableTetrahedra[1] = tetrahedron;
        
        _tetrahedronSwap.tetrahedraAdded++;
    } else {
        throw;
    }
}

void EditController::handleSwapTetrahedron1()
{
    if (_tetrahedronSwap.tetrahedraAdded < 2) {
        return;
    }
    if (_tetrahedronSwap.tetrahedraAdded == 2) {
        
    } else {
        
    }
    handleSwapTetrahedron2();
//    int newUnique = _swappableTetrahedronPoints[0];
//    _swappableTetrahedronPoints[0] = _swappableTetrahedronPoints[1];
//    _swappableTetrahedronPoints[1] = _swappableTetrahedronPoints[2];
//    _swappableTetrahedronPoints[2] = _swappableTetrahedronPoints[4];
//    _swappableTetrahedronPoints[4] = newUnique;
//
//    _mesh->editTetrahedron(_swappableTetrahedra[0],
//                           _swappableTetrahedronPoints[0],
//                           _swappableTetrahedronPoints[1],
//                           _swappableTetrahedronPoints[2],
//                           _swappableTetrahedronPoints[3]);
}

void EditController::handleSwapTetrahedron2()
{
    if (_tetrahedronSwap.tetrahedraAdded < 2) {
        return;
    }
    if (_tetrahedronSwap.tetrahedraAdded == 2) {
        _tetrahedronSwap.swappableTetrahedra[2] = _mesh->addTetrahedron(_tetrahedronSwap.swappableTetrahedronPoints[0],
                                                        _tetrahedronSwap.swappableTetrahedronPoints[1],
                                                        _tetrahedronSwap.swappableTetrahedronPoints[3],
                                                        _tetrahedronSwap.swappableTetrahedronPoints[4]);
        _mesh->editTetrahedron(_tetrahedronSwap.swappableTetrahedra[0],
                               _tetrahedronSwap.swappableTetrahedronPoints[0],
                               _tetrahedronSwap.swappableTetrahedronPoints[2],
                               _tetrahedronSwap.swappableTetrahedronPoints[3],
                               _tetrahedronSwap.swappableTetrahedronPoints[4]);
        _mesh->editTetrahedron(_tetrahedronSwap.swappableTetrahedra[1],
                               _tetrahedronSwap.swappableTetrahedronPoints[1],
                               _tetrahedronSwap.swappableTetrahedronPoints[2],
                               _tetrahedronSwap.swappableTetrahedronPoints[3],
                               _tetrahedronSwap.swappableTetrahedronPoints[4]);
        
        _tetrahedronSwap.tetrahedraAdded++;
    } else {
        _mesh->removeTetrahedron(_tetrahedronSwap.swappableTetrahedra[2]);
        _mesh->editTetrahedron(_tetrahedronSwap.swappableTetrahedra[0],
                               _tetrahedronSwap.swappableTetrahedronPoints[0],
                               _tetrahedronSwap.swappableTetrahedronPoints[1],
                               _tetrahedronSwap.swappableTetrahedronPoints[2],
                               _tetrahedronSwap.swappableTetrahedronPoints[3]);
        _mesh->editTetrahedron(_tetrahedronSwap.swappableTetrahedra[1],
                               _tetrahedronSwap.swappableTetrahedronPoints[0],
                               _tetrahedronSwap.swappableTetrahedronPoints[1],
                               _tetrahedronSwap.swappableTetrahedronPoints[2],
                               _tetrahedronSwap.swappableTetrahedronPoints[4]);
        
        _tetrahedronSwap.tetrahedraAdded--;
    }
//    int newUnique = _swappableTetrahedronPoints[0];
//    _swappableTetrahedronPoints[0] = _swappableTetrahedronPoints[1];
//    _swappableTetrahedronPoints[1] = _swappableTetrahedronPoints[2];
//    _swappableTetrahedronPoints[2] = _swappableTetrahedronPoints[3];
//    _swappableTetrahedronPoints[3] = newUnique;
//
//    _mesh->editTetrahedron(_swappableTetrahedra[1],
//                           _swappableTetrahedronPoints[0],
//                           _swappableTetrahedronPoints[1],
//                           _swappableTetrahedronPoints[2],
//                           _swappableTetrahedronPoints[4]);
}

void EditController::handleSwapTetrahedronHighlight()
{
    GLuint VAO = _mesh->getVAO(RenderMode::cells);
    
    switch (_tetrahedronSwap.tetrahedraAdded) {
        case 0:
            handleHoverTetrahedron();
            break;
        case 1:
            handleHoverTetrahedron();
//            _renderer->submitHighlight(VAO, _tetrahedronSwap.swappableTetrahedra[0], RenderMode::cells);
            break;
        case 2:
//            _renderer->submitHighlight(VAO, _tetrahedronSwap.swappableTetrahedra[0], RenderMode::cells);
//            _renderer->submitHighlight(VAO, _tetrahedronSwap.swappableTetrahedra[1], RenderMode::cells);
            break;
            
        case 3:
//            _renderer->submitHighlight(VAO, _tetrahedronSwap.swappableTetrahedra[0], RenderMode::cells);
//            _renderer->submitHighlight(VAO, _tetrahedronSwap.swappableTetrahedra[1], RenderMode::cells);
//            _renderer->submitHighlight(VAO, _tetrahedronSwap.swappableTetrahedra[2], RenderMode::cells);
            break;
            
        default:
            break;
    }
}
 */

void EditController::undo()
{
    if (_history.currentIndex <= 1) {
        return;
    }
    if (_history.currentIndex == _history.editHistory.size()) {
        logMesh();
        _history.currentIndex--;
    }
    _history.currentIndex--;
    *_mesh = _history.editHistory[_history.currentIndex-1];
    _mesh->reloadBuffers();
}

void EditController::redo()
{
    if (_history.currentIndex == _history.editHistory.size()) {
        return;
    }
    *_mesh = _history.editHistory[_history.currentIndex];
    _mesh->reloadBuffers();
    _history.currentIndex++;
}

void EditController::logMesh()
{
    if (_history.currentIndex != _history.editHistory.size()) {
        _history.editHistory.erase(_history.editHistory.begin() + _history.currentIndex, _history.editHistory.end());
    }
    _history.currentIndex++;
    _history.editHistory.push_back(Mesh(*_mesh));
    assert(_history.currentIndex == _history.editHistory.size());
}

void EditController::clearHistory()
{
    _history.editHistory.clear();
    _history.currentIndex = 0;
}

/*
 abcd
 abce
 abdf
 
 -abc -abd  acd  bcd
 -abc  abe  ace  bce
 -abd  abf  adf  bdf
 
 2 * abc
 2 * abd
 
 abe abf
 acd ace adf
 bcd bce bdf
 
 acd ace
 
 +acd
 +acf
 
 abcd
 abce
 acdf
 
 +ade
 +acf
 
 abcd
 acde
 bcdf
 
  abc  abd -acd -bcd
 -acd  ace  ade  cde
 -bcd  bcf  bdf  cdf
 
 
 */
