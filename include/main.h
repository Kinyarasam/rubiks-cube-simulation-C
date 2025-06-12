#ifndef __MAIN_H__

#define __MAIN_H__

#include <stdio.h>
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <stdbool.h>
#include "cube.h"

#define WIDTH 1000
#define HEIGHT 800
#define CUBELET_COUNT 27

typedef enum {
    FACE_FRONT = 0,
    FACE_BACK = 1,
    FACE_LEFT = 2,
    FACE_RIGHT = 3,
    FACE_BOTTOM = 4,
    FACE_TOP = 5,
} FaceID;

typedef struct {
    SDL_Window* window;
    SDL_GLContext context;
    GLuint shaderProgram;
} Scene;

typedef struct {
    int cubeCount;
    Cubelet cubelets[CUBELET_COUNT];
    bool isRotating;
    bool rotating_clockwise;
    vec3 rotating_axis;
    float rotation_progress;
} Cube;

typedef struct {
    Scene *scene;

    Cube *cube;

    bool isActive;
    SDL_Event event;
    GLuint VAO, VBO, faceIndexBuffer;
} State;

void initCubelets(State* state);
void startFaceRotation(State* state, int face_index, bool clockwise);
void updateCubelets(State* state);
void rotateFaceColors(Cubelet* c, vec3 axis, bool clockwise);

#endif  /** __MAIN_H__ */
