//
//  math4d.cpp
//  4d Object Editor
//
//  Created by David on 06.11.22.
//

#include "math4d.hpp"

//template<typename T>
//void print_mat(T m, const char* name)
//{
//    std::cout << (name ? name : "") << ": " << to_string(m) << std::endl;
//}

#define ZERO_OR_NAN(a) (std::isnan(a) || a == 0)

#define SAME_OR_NAN(a, b) (std::isnan(a) || std::isnan(b) || a == b || a == 0 || b == 0 || std::isinf(a) || std::isinf(b))

#define IS_CONSTANT(vec) SAME_OR_NAN(vec.x, vec.y) && SAME_OR_NAN(vec.z, vec.w) && SAME_OR_NAN(vec.x, vec.z) && SAME_OR_NAN(vec.y, vec.w)

//#define IS_CONSTANT(vec) (vec.x == vec.y && vec.x == vec.z && vec.x == vec.w) || (ZERO_OR_NAN(vec.x) && ZERO_OR_NAN(vec.y) && ZERO_OR_NAN(vec.z) && ZERO_OR_NAN(vec.w))

//#define IS_CONSTANT(vec) (vec.x == vec.y && vec.x == vec.z && vec.x == vec.w) || std::isnan(vec.x) || std::isnan(vec.y) || std::isnan(vec.z) || std::isnan(vec.w)

//#define IS_CONSTANT(vec) (vec.x == vec.y && vec.x == vec.z && vec.x == vec.w)

void rotatePlane(vec4& a, vec4& b, float angle)
{
    ASSERT_PERP(a, b);
    
    vec4 na = cos(angle) * a + sin(angle) * b;
    vec4 nb = cos(angle) * b - sin(angle) * a;
    
    a = na;
    b = nb;
}

void rotatePlane(const vec4& a, const vec4& b, vec4& outA, vec4& outB, float angle)
{
    ASSERT_PERP(a, b);
    
    vec4 na = cos(angle) * a + sin(angle) * b;
    vec4 nb = cos(angle) * b - sin(angle) * a;
    
    outA = na;
    outB = nb;
}

vec4 perpendicular(vec4 a, vec4 b, vec4 c)
{
    //    float x = determinant(mat4(a, b, c, vec4(1, 0, 0, 0)));
    //    float y = determinant(mat4(a, b, c, vec4(0, 1, 0, 0)));
    //    float z = determinant(mat4(a, b, c, vec4(0, 0, 1, 0)));
    //    float w = determinant(mat4(a, b, c, vec4(0, 0, 0, 1)));
    float detXY = a.x * b.y - a.y * b.x;
    float detZW = a.z * b.w - a.w * b.z;
    float detXZ = a.x * b.z - a.z * b.x;
    float detYW = a.y * b.w - a.w * b.y;
    float detXW = a.x * b.w - a.w * b.x;
    float detYZ = a.y * b.z - a.z * b.y;
    
    float x = c.y * detZW - c.z * detYW + c.w * detYZ;
    float y = -(c.x * detZW - c.z * detXW + c.w * detXZ);
    float z = c.x * detYW - c.y * detXW + c.w * detXY;
    float w = -(c.x * detYZ - c.y * detXZ + c.z * detXY);
    
    vec4 result = vec4(x, y, z, w);
    
    if (result == vec4(0)) {
        vec4 ratAB = a / b;
        vec4 ratBC = b / c;
        vec4 ratAC = c / a;
        
        if (IS_CONSTANT(ratAB)) {
            result = perpendicular(b, c, normalize(vec4(rand(), rand(), rand(), rand())));
        } else if (IS_CONSTANT(ratBC)) {
            result = perpendicular(c, a, normalize(vec4(rand(), rand(), rand(), rand())));
        } else if (IS_CONSTANT(ratAC)) {
            result = perpendicular(b, a, normalize(vec4(rand(), rand(), rand(), rand())));
        } else {
            errno;
        }
        /*
         vec4 ratAC = c / a;
         vec4 ratBC = b / c;
         vec4 ratAB = a / b;
         
         if (IS_CONSTANT(ratAC)) {
         result = perpendicular(b, a, normalize(vec4(rand(), rand(), rand(), rand())));
         } else if (IS_CONSTANT(ratAB)) {
         result = perpendicular(b, c, normalize(vec4(rand(), rand(), rand(), rand())));
         } else
         //  if (IS_CONSTANT(ratBC))
         {
         result = perpendicular(a, c, normalize(vec4(rand(), rand(), rand(), rand())));
         }
         if (result == vec4(0)) {
         printf("");
         }
         //        ASSERT_PERP(result, a);
         //        ASSERT_PERP(result, b);
         //        ASSERT_PERP(result, c);
         */
    }
    //    ASSERT_PERP(result, a);
    //    ASSERT_PERP(result, b);
    //    ASSERT_PERP(result, c);
    
    return result;
}


vec4 simplexPerpendicular(vec4 a, vec4 b, vec4 c, vec4 d)
{
    return normalize(perpendicular(a - d, b - d, c - d));
}

vec4 trianglePerpendicularHelper(vec4 a, vec4 b, vec4 c, vec4 helperDir)
{
    return perpendicular(b - a, c - a, helperDir);
}

bool conatinsOrigin(vec4 a, vec4 b, vec4 c, vec4 d, vec4 perp)
{
    vec4 dir;
    float dist;
    float other_dist;
    
    dir = trianglePerpendicularHelper(a, b, c, perp);
    dist = dot(dir, a);
    other_dist = dot(dir, d);
    if (sign(dist) == sign(other_dist)) {
        return false;
    }
    
    dir = trianglePerpendicularHelper(a, b, d, perp);
    dist = dot(dir, a);
    other_dist = dot(dir, c);
    if (sign(dist) == sign(other_dist)) {
        return false;
    }
    
    dir = trianglePerpendicularHelper(a, c, d, perp);
    dist = dot(dir, a);
    other_dist = dot(dir, b);
    if (sign(dist) == sign(other_dist)) {
        return false;
    }
    
    dir = trianglePerpendicularHelper(b, c, d, perp);
    dist = dot(dir, b);
    other_dist = dot(dir, a);
    if (sign(dist) == sign(other_dist)) {
        return false;
    }
    
    return true;
}


float raySphereIntersect(vec4 center, float radius, vec4 start, vec4 direction, vec4 perp)
{
    direction = normalize(direction);
    
    vec4 dif = center - start;
    
    dif -= dot(perp, dif) * perp;
    
    float len = dot(dif, direction);
    if (len < 0) {
        return len;
    }
    vec4 proj = dot(direction, dif) * direction;
    
    if (distance(proj, dif) < radius) {
        return len;
    }
    return -distance(proj, dif);
}

float rayLineIntersect(vec4 a, vec4 b, vec4 start, vec4 direction, vec4 perp, float maxAllowed)
{
    throw;
    return 0;
    
    direction = normalize(direction);
    
    vec4 dif_a = a - start;
    dif_a -= dot(perp, dif_a) * perp;
    
    vec4 dif_b = b - start;
    dif_b -= dot(perp, dif_b) * perp;
    
    vec4 dif_ab = dif_b - dif_a;
    
    vec4 helperPerp = perpendicular(dif_a, dif_b, direction);
    vec4 linePerp = normalize(perpendicular(helperPerp, dif_ab, direction));
//    ASSERT_APPROX(perp, linePerp);
    
    float distDif = dot(dif_a, linePerp);
    
    if (abs(distDif) > maxAllowed) {
        return 0;
    }
    
//    dif_a -= linePerp * distDif;
//    dif_b -= linePerp * distDif;
//
//    float startDist = dot(direction, dif_a);
//
    
    vec4 directionPerp = perpendicular(direction, linePerp, helperPerp);
    
    float off_a = dot(directionPerp, a);
    float off_b = dot(directionPerp, b);
    
    if (sign(off_a) == sign(off_b)) {
        return 0;
    }
    
    return mix(dot(direction, dif_a),
               dot(direction, dif_b),
               abs(off_a) / (abs(off_a) + abs(off_b))
               );
}

float raySimplexIntersect(vec4 a,
                          vec4 b,
                          vec4 c,
                          vec4 d,
                          vec4 start,
                          vec4 direction)
{
 
    
    vec4 perp = simplexPerpendicular(a, b, c, d);
    
    float simplexDist = dot(perp, a);
    float pointDist = dot(perp, start);
    if (simplexDist > pointDist) {
        simplexDist *= -1;
        pointDist *= -1;
        perp *= -1;
    }
    
    float alignment = dot(perp, direction);
    if (alignment >= 0) {
        return 0;
    }
    
    
    float dist = (simplexDist - pointDist) / alignment;
    
    start += direction * dist;
    
    a -= start;
    b -= start;
    c -= start;
    d -= start;
    
    if (conatinsOrigin(a, b, c, d, perp)) {
        return dist;
    }
    return 0;
    
}
