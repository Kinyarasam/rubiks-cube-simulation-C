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
        free(gameState);
        SDL_Quit();
        fprintf(stderr, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return NULL;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GLContext context = SDL_GL_CreateContext(window);
    if (!context) {
        SDL_DestroyWindow(window);
        free(gameState);
        SDL_Quit();
        fprintf(stderr, "OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
        return NULL;
    }

    glViewport(0, 0, WIDTH, HEIGHT);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        SDL_GL_DeleteContext(context);
        SDL_DestroyWindow(window);
        free(gameState);
        SDL_Quit();
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
        SDL_GL_DeleteContext(context);
        SDL_DestroyWindow(window);
        free(gameState);
        SDL_Quit();
        return NULL;
    }

    gameState->scene = malloc(sizeof(Scene));
    if (!gameState->scene) {
        fprintf(stderr, "Failed to allocate Scene\n");
        glDeleteProgram(shaderProgram);
        SDL_GL_DeleteContext(context);
        SDL_DestroyWindow(window);
        free(gameState);
        SDL_Quit();
        return NULL;
    }

    gameState->scene->window = window;
    gameState->scene->context = context;
    gameState->scene->shaderProgram = shaderProgram;

    gameState->cube = malloc(sizeof(Cube));
    if (!gameState->cube) {
        fprintf(stderr, "Failed to allocate Cube\n");
        glDeleteProgram(shaderProgram);
        SDL_GL_DeleteContext(context);
        SDL_DestroyWindow(window);

        free(gameState->scene);
        free(gameState);
        SDL_Quit();
        return NULL;
    }

    initCubelets(gameState);

    return gameState;
}

void cleanup(State* state) {
    glDeleteVertexArrays(1, &state->VAO);
    glDeleteBuffers(1, &state->VBO);
    glDeleteBuffers(1, &state->faceIndexBuffer);
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
    float positions[6*6*3] = {
        // Front face
        -0.5f, -0.5f,  0.5f,
        0.5f, -0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        
        // Back face
        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        
        // Left face
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        
        // Right face
        0.5f,  0.5f,  0.5f,
        0.5f,  0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,
        
        // Bottom face
        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f,  0.5f,
        0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,
        
        // Top face
        -0.5f,  0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,
        0.5f,  0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f
    };

    int faceIndices[36] = {
        FACE_FRONT, FACE_FRONT, FACE_FRONT, FACE_FRONT, FACE_FRONT, FACE_FRONT,  // Front (Red)
        FACE_BACK, FACE_BACK, FACE_BACK, FACE_BACK, FACE_BACK, FACE_BACK,  // Back (Orange)
        FACE_LEFT, FACE_LEFT, FACE_LEFT, FACE_LEFT, FACE_LEFT, FACE_LEFT,  // Left (Green)
        FACE_RIGHT, FACE_RIGHT, FACE_RIGHT, FACE_RIGHT, FACE_RIGHT, FACE_RIGHT,  // Right (Blue)
        FACE_BOTTOM, FACE_BOTTOM, FACE_BOTTOM, FACE_BOTTOM, FACE_BOTTOM, FACE_BOTTOM,  // Bottom (White)
        FACE_TOP, FACE_TOP, FACE_TOP, FACE_TOP, FACE_TOP, FACE_TOP   // Top (Yellow)
    };

    glGenVertexArrays(1, &state->VAO);
    glGenBuffers(1, &state->VBO);
    glGenBuffers(1, &state->faceIndexBuffer);

    glBindVertexArray(state->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, state->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, state->faceIndexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(faceIndices), faceIndices, GL_STATIC_DRAW);
    glVertexAttribIPointer(2, 1, GL_INT, 0, (void*)0);
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    while (state->isActive) {
        while (SDL_PollEvent(&state->event)) {
            handleInput(state);
        }

        updateCubelets(state);

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

        glBindVertexArray(state->VAO);
        for (int i = 0; i < CUBELET_COUNT; i++) {
            Cubelet* cubelet = &state->cube->cubelets[i];

            mat4 model;
            glm_mat4_identity(model);

            float angle = glm_vec3_norm(cubelet->rotating_angle);
            if (angle > 0.0f) {
                vec3 axis;
                glm_vec3_copy(cubelet->rotating_angle, axis);
                glm_vec3_normalize(axis); // You naughty lil angle, behave! 😤
                glm_rotate(model, angle, axis); // YAS twist that body 😩
            }

            glm_translate(model, cubelet->position);

            GLuint colorsLoc = glGetUniformLocation(state->scene->shaderProgram, "faceColors");
            glUniform3fv(colorsLoc, 6, (float*)cubelet->face_colors);

            GLuint modelLoc = glGetUniformLocation(state->scene->shaderProgram, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (float*)model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glBindVertexArray(0);

        SDL_GL_SwapWindow(state->scene->window);
    }

    cleanup(state);
}