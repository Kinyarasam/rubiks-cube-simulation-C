#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "utils.h"
#include "events.h"

State* initializeState() {
    State* gameState = malloc(sizeof(State));

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return NULL;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Rubik's Cube Simulation",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WIDTH, HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
    );
    if (!window) {
        fprintf(stderr, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return NULL;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GLContext context = SDL_GL_CreateContext(window);
    if (!context) {
        fprintf(stderr, "OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
        return NULL;
    }

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return NULL;
    }

    glEnable(GL_DEPTH_TEST);

     GLuint shaderProgram = createShaderProgram(
        "shaders/vertex.glsl",
        "shaders/fragment.glsl"
    );
    if (!shaderProgram) {
        fprintf(stderr, "Failed to create shader program.\n");
        return NULL;
    }

    gameState->scene = malloc(sizeof(Scene));
    if (!gameState->scene) {
        fprintf(stderr, "Failed to allocate Scene\n");
        return NULL;
    }

    gameState->scene->window = window;
    gameState->scene->context = context;
    gameState->scene->shaderProgram = shaderProgram;

    return gameState;
}

void cleanup(State* state) {
    glDeleteProgram(state->scene->shaderProgram);

    SDL_GL_DeleteContext(state->scene->context);
    SDL_DestroyWindow(state->scene->window);
    SDL_Quit();

    free(state->scene);
    free(state);
}

int main(int argc, char* argv[]) {
    State* state = initializeState();
    if (!state) {
        return 1;
    }

    state->isActive = true;

    while (state->isActive) {
        while (SDL_PollEvent(&state->event)) {
            handleInput(state);
        }
    }

    cleanup(state);
}