//
//  math4d.hpp
//  4d Object Editor
//
//  Created by David on 06.11.22.
//

#ifndef math4d_hpp
#define math4d_hpp

#include <iostream>

#include "glm/glm.hpp"

using namespace glm;

// epsilon for proximity testing
#define EPS 0.00001

#define ASSERT_PERP(a, b) assert(abs(dot(a, b)) <= EPS * length(a) * length(b))
//{float dif = (dot(a, b));if(abs(dif > EPS)){std::cout << dif << std::endl;}}
// assert(abs(dot(a, b)) < EPS)
#define ASSERT_APPROX(a, b) assert(abs(a - b) < EPS)
#define ASSERT_NORMALIZED(a) assert(abs(length(a) - 1) < EPS)


/// Rotate vectors in a plane (in and out variables may be the same)
/// - Parameters:
///   - a: the first direction vector defining the plane
///   - b: the second direction vector defining the plane; must be perpendicular to a
///   - outA: output for rotated a
///   - outB: output for rotated b
///   - angle: angle to rotate in radians
void rotatePlane(const vec4& a, const vec4& b, vec4& outA, vec4& outB, float angle);


/// Rotate vectors in plane defined by them
/// - Parameters:
///   - a: first vector
///   - b: second vector
///   - angle: angle to rotate
void rotatePlane(vec4& a, vec4& b, float angle);

template<typename T>
void print_mat(T m, const char* name)
{
    std::cout << (name ? name : "") << ": " << to_string(m) << "\n";
}

/// get the vector perpendicular to the other two
/// - Parameters:
///   - a: first vector
///   - b: second vector
///   - c: third vector
vec4 perpendicular(vec4 a, vec4 b, vec4 c);

/// get the perpendicular vetor to the simplex (tetrahedron) defined by the vector
/// - Parameters:
///   - a: first vertex of tetrahedron
///   - b: second vertex
///   - c: third vertex
///   - d: fourth vertex
vec4 simplexPerpendicular(vec4 a, vec4 b, vec4 c, vec4 d);

bool conatinsOrigin(vec4 a, vec4 b, vec4 c, vec4 d, vec4 perp);

float raySphereIntersect(vec4 center, float radius, vec4 start, vec4 direction, vec4 perp);

float rayLineIntersect(vec4 a, vec4 b, vec4 start, vec4 direction, vec4 perp, float maxAllowed);

float raySimplexIntersect(vec4 a,
                          vec4 b,
                          vec4 c,
                          vec4 d,
                          vec4 start,
                          vec4 direction);

#endif /* math4d_hpp */
