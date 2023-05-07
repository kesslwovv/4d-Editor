//
//  meshEditor.hpp
//  4d Object Editor
//
//  Created by David on 26.02.23.
//

#ifndef meshEditor_hpp
#define meshEditor_hpp

#include <stdio.h>

// this file lists all deprecated edit mesh functionality

/*
void editVertex(int idx, vec4 value);
int addVertex(vec4 value);
void removeVertex(int idx);

int addEdge(int idx1, int idx2);
tup2<int> removeEdge(int idx);

int addTriangle(int idx1, int idx2, int idx3);
tup3<int> removeTriangle(int idx);

int addTetrahedron(int a, int b, int c, int d);
void addTetrahedralConnections(int a, int b, int c, int d);
tup4<int> removeTetrahedron(int idx);
void editTetrahedron(int idx, int a, int b, int c, int d);

tup2<vec4> getEdge(int idx);
tup4<vec4> getTetrahedron(int idx);
tup4<int> getTetrahedronIndices(int idx);

void insertPoint(int point, int tetrahedronIndex);

void cullDuplicateTetrahedra();
void rebuildConnectionsFromTetrahedra();

bool edgeExists(int arr[2]);
bool triangleExists(int arr[3]);
 */

#endif /* meshEditor_hpp */
