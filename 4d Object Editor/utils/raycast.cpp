//
//  raycast.cpp
//  4d Object Editor
//
//  Created by David on 07.04.23.
//

#include "raycast.hpp"

float intersect(AABB box, Ray r)
{
    float tMin = -FLT_MAX;
    float tMax = FLT_MAX;
    
    if (r.direction.x != 0.0f) {
        float hit1 = (box.lo.x - r.origin.x) / r.direction.x;
        float hit2 = (box.hi.x - r.origin.x) / r.direction.x;

        tMin = max(tMin, min(hit1, hit2));
        tMax = min(tMax, max(hit1, hit2));
    }

    if (r.direction.y != 0.0f) {
        float hit1 = (box.lo.y - r.origin.y) / r.direction.y;
        float hit2 = (box.hi.y - r.origin.y) / r.direction.y;

        tMin = max(tMin, min(hit1, hit2));
        tMax = min(tMax, max(hit1, hit2));
    }
    
    if (r.direction.z != 0.0f) {
        float hit1 = (box.lo.z - r.origin.z) / r.direction.z;
        float hit2 = (box.hi.z - r.origin.z) / r.direction.z;

        tMin = max(tMin, min(hit1, hit2));
        tMax = min(tMax, max(hit1, hit2));
    }
    
    if (r.direction.w != 0.0f) {
        float hit1 = (box.lo.w - r.origin.w) / r.direction.w;
        float hit2 = (box.hi.w - r.origin.w) / r.direction.w;

        tMin = max(tMin, min(hit1, hit2));
        tMax = min(tMax, max(hit1, hit2));
    }
    
    
    if (tMin > tMax || tMax < 0) {
        return FLT_MAX;
    }
    if (tMin > 0) {
        return tMin;
    }
    return tMax;

}

