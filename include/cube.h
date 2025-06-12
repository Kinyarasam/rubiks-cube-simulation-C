#ifndef __CUBE_H__
#define __CUBE_H__

#include <cglm/cglm.h>

typedef struct{
    vec3 position;
    vec3 rotating_angle;
    vec3 face_colors[6]; 
} Cubelet;

#endif  /** __CUBE_H__ */
