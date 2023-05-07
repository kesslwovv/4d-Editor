//
//  meshEditor.cpp
//  4d Object Editor
//
//  Created by David on 26.02.23.
//

#include "meshEditor.hpp"

// this file contains all implementation for these functionalities

/*
 void EditableMesh::editVertex(int idx, vec4 value)
 {
     _vertecies[idx] = value;
     reloadVBO();
 }

 int EditableMesh::addVertex(vec4 value)
 {
     int idx = (int)_vertecies.size();
     _vertecies.push_back(value);
     reloadVBO();
     return idx;
 }

 void EditableMesh::removeVertex(int idx)
 {
     // TODO: remove all references. this is janky hack.
     int targetIdx = (int)_vertecies.size() - 1;
     for (int i = (int)_edge_count - 1;  i >= 0; i--) {
         if (contains<2>(&_edges[i*2], (GLuint)idx)) {
             _edges.erase(_edges.begin()+i*2, _edges.begin()+i*2+2);
         } else {
             replace<2>((int*)&_edges[i*2], idx, targetIdx);
         }
     }

     for (int i = (int)_triangle_count - 1;  i >= 0; i--) {
         if (contains<3>(&_triangles[i*3], (GLuint)idx)) {
             _triangles.erase(_triangles.begin()+i*3, _triangles.begin()+i*3+3);
         } else {
             replace<3>((int*)&_triangles[i*3], idx, targetIdx);
         }
     }
     
     for (int i = (int)_tetrahedra_count - 1;  i >= 0; i--) {
         if (contains<4>(&_tetrahedra[i*4], (GLuint)idx)) {
             _tetrahedra.erase(_tetrahedra.begin()+i*4, _tetrahedra.begin()+i*4+4);
         } else {
             replace<4>((int*)&_tetrahedra[i*4], idx, targetIdx);
         }
     }
     
     _vertecies[idx] = _vertecies[_vertecies.size() - 1];
     _vertecies.pop_back();
 //    reloadVBO();
     reloadBuffers();
 }

 int EditableMesh::addEdge(int idx1, int idx2)
 {
     int idx = (int)_edges.size() / 2;
     _edges.push_back(idx1);
     _edges.push_back(idx2);
     reloadEdges();
     return idx;
 }

 tup2<int> EditableMesh::removeEdge(int idx)
 {
     tup2<int> result{_edges[idx*2], _edges[idx*2+1]};
     _edges.erase(_edges.begin() + idx*2, _edges.begin() + idx*2 + 2);
     reloadEdges();
     return result;
 }

 int EditableMesh::addTriangle(int idx1, int idx2, int idx3)
 {
     int idx = (int)_triangles.size() / 3;
     _triangles.push_back(idx1);
     _triangles.push_back(idx2);
     _triangles.push_back(idx3);
     reloadFaces();
     return idx;
 }

 tup3<int> EditableMesh::removeTriangle(int idx)
 {
     tup3<int> result{_triangles[idx*3], _triangles[idx*3+1], _triangles[idx*3+2]};
     _triangles.erase(_triangles.begin() + idx*3, _triangles.begin() + idx*3 + 3);
     reloadFaces();
     return result;
 }


 tup2<vec4> EditableMesh::getEdge(int idx)
 {
     size_t idx1 = _edges[idx*2];
     size_t idx2 = _edges[idx*2+1];
     return std::pair<vec4, vec4>(_vertecies[idx1], _vertecies[idx2]);
 }

 tup4<vec4> EditableMesh::getTetrahedron(int idx)
 {
     vec4 a = _vertecies[_tetrahedra[idx*4]];
     vec4 b = _vertecies[_tetrahedra[idx*4+1]];
     vec4 c = _vertecies[_tetrahedra[idx*4+2]];
     vec4 d = _vertecies[_tetrahedra[idx*4+3]];
     return {a, b, c, d};
 }

 int EditableMesh::addTetrahedron(int a, int b, int c, int d)
 {
 //    return 0;
     int idx = (int)_tetrahedra.size() / 4;
     _tetrahedra.push_back(a);
     _tetrahedra.push_back(b);
     _tetrahedra.push_back(c);
     _tetrahedra.push_back(d);
     
     reloadCells();
     
     return idx;
 }

 void EditableMesh::addTetrahedralConnections(int a, int b, int c, int d)
 {
     addTetrahedron(a, b, c, d);
     
 //    addTriangle(a, b, c);
 //    addTriangle(a, b, d);
 //    addTriangle(a, d, c);
 //    addTriangle(d, b, c);
 //
     addEdge(a, b);
     addEdge(a, c);
     addEdge(a, d);
     addEdge(b, c);
     addEdge(b, d);
     addEdge(c, d);
 }

 void EditableMesh::insertPoint(int point, int tetrahedronIndex)
 {

     tup4<int> tetrahedron = getTetrahedronIndices(tetrahedronIndex);
     int& a = get<0>(tetrahedron);
     int& b = get<1>(tetrahedron);
     int& c = get<2>(tetrahedron);
     int& d = get<3>(tetrahedron);
     
     addTetrahedron(a, b, c, point);
     addTetrahedron(a, b, d, point);
     addTetrahedron(a, c, d, point);
     addTetrahedron(b, c, d, point);
     
     checkError();

     addTriangle(a, b, point);
     addTriangle(a, c, point);
     addTriangle(a, d, point);
     addTriangle(b, c, point);
     addTriangle(b, d, point);
     addTriangle(c, d, point);
     
     addEdge(a, point);
     addEdge(b, point);
     addEdge(c, point);
     addEdge(d, point);
     
     removeTetrahedron(tetrahedronIndex);
 }

 tup4<int> EditableMesh::removeTetrahedron(int idx)
 {
     tup4<int> result(_tetrahedra[idx*4],
                                           _tetrahedra[idx*4+1],
                                           _tetrahedra[idx*4+2],
                                           _tetrahedra[idx*4+3]);
     _tetrahedra.erase(_tetrahedra.begin() + idx * 4, _tetrahedra.begin() + idx * 4 + 4);
     
     reloadCells();
     
     return result;
 }

 void EditableMesh::cullDuplicateTetrahedra()
 {
     for (int i = (int)_tetrahedra.size() / 4 - 1; i >= 0; i--) {
         unsigned int* comp = &_tetrahedra[i*4];
         for (int j = i - 1; j >= 0; j--) {
             if (equal<4>(&_tetrahedra[j*4], comp)) {
                 _tetrahedra.erase(_tetrahedra.begin() + i*4, _tetrahedra.begin() + i*4 + 4);
                 _tetrahedra.erase(_tetrahedra.begin() + j*4, _tetrahedra.begin() + j*4 + 4);
                 i--;
                 break;
             }
         }
     }
     reloadCells();
 }

 void EditableMesh::rebuildConnectionsFromTetrahedra()
 {
     _triangles.clear();
     _edges.clear();
     for (int i = 0; i < _tetrahedra.size() / 4; i++) {
 #define ADD_EDG_IF_NOT_EXISTS(a, b) {int edge[2] {(int)_tetrahedra[i*4+a], (int)_tetrahedra[i*4+b]}; if(!edgeExists(edge)){addEdge(edge[0], edge[1]);}}
         ADD_EDG_IF_NOT_EXISTS(0, 1);
         ADD_EDG_IF_NOT_EXISTS(0, 2);
         ADD_EDG_IF_NOT_EXISTS(0, 3);
         ADD_EDG_IF_NOT_EXISTS(1, 2);
         ADD_EDG_IF_NOT_EXISTS(1, 3);
         ADD_EDG_IF_NOT_EXISTS(2, 3);
 //        {int edge[2] {(int)_tetrahedra[i*4+0], (int)_tetrahedra[i*4+1]}; if(!edgeExists(edge)){addEdge(edge[0], edge[1]);}}
 #undef ADD_EDG_IF_NOT_EXISTS
     }
     
 }

 bool EditableMesh::triangleExists(int arr[3])
 {
     for (int i = 0; i < _triangles.size() / 3; i++) {
         if (isPerm<3>((int*)&_triangles[i*3], arr)) {
             return true;
         }
     }
     return false;
 }

 bool EditableMesh::edgeExists(int arr[2])
 {
     for (int i = 0; i < _edges.size() / 2; i++) {
         if (isPerm<2>((int*)&_edges[i*2], arr)) {
             return true;
         }
     }
     return false;
 }

 void EditableMesh::editTetrahedron(int idx, int a, int b, int c, int d)
 {
     _tetrahedra[idx*4] = a;
     _tetrahedra[idx*4+1] = b;
     _tetrahedra[idx*4+2] = c;
     _tetrahedra[idx*4+3] = d;
     
     reloadCells();
 }

 tup4<int> EditableMesh::getTetrahedronIndices(int idx)
 {
     tup4<int> result(_tetrahedra[idx*4],
                     _tetrahedra[idx*4+1],
                     _tetrahedra[idx*4+2],
                      _tetrahedra[idx*4+3]);
     return result;
 }

 */
