#  File formats

This Program uses some custom file formats:

- .obj4d
- .obj4d.bin
- .obj4d.colordata
- .obj4d.colordata.bin

- .texture

# .obj4d

text format  

header (4 lines):  
1 line (int): v = number of vertecies  
1 line (int): e = number of edges  
1 line (int): f = number of faces (triangles)  
1 line (int): c = number of cells (tetrahedra)  

body:  
v lines:  
(float, float, float, float): coordinates of vertex  
e lines:  
(int, int): indices of vertecies connected by an edge  
f lines:  
(int, int, int): indices of vertecies forming a face  
c lines:  
(int, int, int, int): indices of vertecies forming a tetrahedron  


<table>
<tr>
<th> plane.obj4d </th>
<th> annotation </th>
</tr>
<tr>
<td>

```
9
12
12
12
1,0,1,1
-1,0,1,1
1,0,-1,1
-1,0,-1,1
1,0,1,-1
-1,0,1,-1
1,0,-1,-1
-1,0,-1,-1
0.0,0.0,0.0,0.0
0,1
0,2
0,4
1,3
1,5
2,3
2,6
3,7
4,5
4,6
5,7
6,7
0,2,3
0,3,1
0,4,5
0,5,1
0,4,6
0,6,2
1,5,7
1,7,3
2,6,7
2,7,3
4,6,7
4,7,5
0,2,3,8
0,3,1,8
2,6,7,8
2,7,3,8
4,6,7,8
4,7,5,8
1,5,7,8
1,7,3,8
0,4,5,8
0,5,1,8
0,4,6,8
0,6,2,8

```

</td>
  
<td>
  
```
number of vertecies  
number of edges  
number of faces (triangles)  
number of cells (tetrahedra)  
\
|
|
|
| vertecies
|
|
|
/
\
|
|
|
|
|
| edges
|
|
|
|
/
\
|
|
|
|
| faces (triangles)
|
|
|
|
|
/
\
|
|
|
|
| cells (tetrahedra)
|
|
|
|
|
/

```

</td>
</tr>
</table>

plane.obj4d:
```
9
12
12
12
1,0,1,1
-1,0,1,1
1,0,-1,1
-1,0,-1,1
1,0,1,-1
-1,0,1,-1
1,0,-1,-1
-1,0,-1,-1
0.0,0.0,0.0,0.0
0,1
0,2
0,4
1,3
1,5
2,3
2,6
3,7
4,5
4,6
5,7
6,7
0,2,3
0,3,1
0,4,5
0,5,1
0,4,6
0,6,2
1,5,7
1,7,3
2,6,7
2,7,3
4,6,7
4,7,5
0,2,3,8
0,3,1,8
2,6,7,8
2,7,3,8
4,6,7,8
4,7,5,8
1,5,7,8
1,7,3,8
0,4,5,8
0,5,1,8
0,4,6,8
0,6,2,8

```

# .obj4d.bin

binary format

the order is the same as in text format:

header (32 byte):  
8 byte (`uint64_t`): v = number of vertecies  
8 byte (`uint64_t`): e = number of edges  
8 byte (`uint64_t`): f = number of faces (triangles)  
8 byte (`uint64_t`): c = number of cells (tetrahedra)  


body:  
v blocks of 16 byte:  
(float, float, float, float): coordinates of vertex  
e blocks of 8 byte:  
(int, int): indices of vertecies connected by an edge  
f blocks of 12 byte:  
(int, int, int): indices of vertecies forming a face  
c blocks of 16 byte:  
(int, int, int, int): indices of vertecies forming a tetrahedron  

# .obj4d.colordata

TODO

# .obj4d.colordata.bin

TODO

# .texture

TODO
