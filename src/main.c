#include <stdio.h>
#include <stdlib.h>
#include <cglm/cglm.h>
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

    initCubelets(gameState);

    return gameState;
}

void cleanup(State* state) {
    glDeleteProgram(state->scene->shaderProgram);

    SDL_GL_DeleteContext(state->scene->context);
    SDL_DestroyWindow(state->scene->window);
    SDL_Quit();

    free(state->scene);
    free(state->cube);
    free(state);
}

int main(int argc, char* argv[]) {
    State* state = initializeState();
    if (!state) {
        return 1;
    }

    state->isActive = true;

    // Vertex data with positions and colors (6 faces × 6 vertices × (3 position + 3 color))
    float vertices[6*6*6] = {
        // Front face (red)
        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
        
        // Back face (orange)
        -0.5f, -0.5f, -0.5f,  1.0f, 0.5f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.5f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 0.5f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 0.5f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 0.5f, 0.0f,
        -0.5f, -0.5f, -0.5f,  1.0f, 0.5f, 0.0f,
        
        // Left face (green)
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
        
        // Right face (blue)
         0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
        
        // Bottom face (white)
        -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f,
        
        // Top face (yellow)
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f
    };

    GLuint VBO, VAO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    while (state->isActive) {
        while (SDL_PollEvent(&state->event)) {
            handleInput(state);
        }

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(state->scene->shaderProgram);

        // Camera setup
        mat4 view, projection;
        glm_mat4_identity(view);
        glm_lookat((vec3){3.0f, 3.0f, 3.0f}, (vec3){0.0f, 0.0f, 0.0f}, (vec3){0.0f, 1.0f,0.0f}, view);
        glm_perspective(glm_rad(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f, projection);

        GLuint viewLoc = glGetUniformLocation(state->scene->shaderProgram, "view");
        GLuint projLoc = glGetUniformLocation(state->scene->shaderProgram, "projection");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (float*)view);
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, (float*)projection);

        glBindVertexArray(VAO);
        for (int i = 0; i < CUBELET_COUNT; i++) {
            mat4 model;
            glm_mat4_identity(model);
            glm_translate(model, state->cube->cubelets[i].position);

            GLuint modelLoc = glGetUniformLocation(state->scene->shaderProgram, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (float*)model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glBindVertexArray(0);

        SDL_GL_SwapWindow(state->scene->window);
    }

    cleanup(state);
}