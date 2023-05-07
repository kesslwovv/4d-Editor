//
//  raycast.hpp
//  4d Object Editor
//
//  Created by David on 07.04.23.
//

#ifndef raycast_hpp
#define raycast_hpp

#include <stdio.h>
#include "glm/glm.hpp"

using namespace glm;


struct Ray {
    vec4 origin;
    vec4 direction;
};

struct AABB {
    vec4 lo;
    vec4 hi;
};


float intersect(AABB box, Ray r);


#endif /* raycast_hpp */
