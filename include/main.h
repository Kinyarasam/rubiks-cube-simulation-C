#ifndef __MAIN_H__

#include <stdio.h>
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <stdbool.h>
#include "cube.h"

#define WIDTH 1000
#define HEIGHT 800
#define CUBELET_COUNT 27

typedef struct {
    SDL_Window* window;
    SDL_GLContext context;
    GLuint shaderProgram;
} Scene;

typedef struct {
    int cubeCount;
    Cubelet cubelets[CUBELET_COUNT];
} Cube;

typedef struct {
    Scene *scene;

    Cube *cube;

    bool isActive;
    SDL_Event event;
} State;

void initCubelets(State* state);

#endif  /** __MAIN_H__ */
