#ifndef __MAIN_H__

#include <stdio.h>
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <stdbool.h>

#define WIDTH 1000
#define HEIGHT 800

typedef struct {
    SDL_Window* window;
    SDL_GLContext context;
    GLuint shaderProgram;
} Scene;

typedef struct {
    Scene *scene;

    bool isActive;
    SDL_Event event;
} State;

#endif  /** __MAIN_H__ */
