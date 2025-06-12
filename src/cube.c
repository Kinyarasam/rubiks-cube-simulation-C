#include <string.h>
#include "main.h"

void initCubelets(State* state) {
    vec3 colors[6] = {
        {1.0f, 0.0f, 0.0f},   // Front (Red)
        {1.0f, 0.5f, 0.0f},   // Back (Orange)
        {0.0f, 1.0f, 0.0f},   // Left (Green)
        {0.0f, 0.0f, 1.0f},   // Right (Blue)
        {1.0f, 1.0f, 1.0f},   // Bottom (White)
        {1.0f, 1.0f, 0.0f}    // Top (Yellow)
    };

    int index = 0;
    for (float x = -1.0f; x <= 1.0f; x += 1.0f) {
        for (float y = -1.0f; y <= 1.0f; y += 1.0f) {
            for (float z = -1.0f; z <= 1.0f; z += 1.0f) {
                Cubelet* cubelet = &state->cube->cubelets[index];
                glm_vec3_copy((vec3){x, y, z}, cubelet->position);
                glm_vec3_zero(cubelet->rotating_angle);

               for (int i = 0; i < 6; i++) {
                    // Default to dark gray for internal faces instead of black
                    glm_vec3_copy((vec3){0.2f, 0.2f, 0.2f}, cubelet->face_colors[i]);
                }

                // Apply colors based on initial position
                if (z == 1.0f) glm_vec3_copy(colors[0], cubelet->face_colors[0]);  // Front
                if (z == -1.0f) glm_vec3_copy(colors[1], cubelet->face_colors[1]); // Back
                if (x == -1.0f) glm_vec3_copy(colors[2], cubelet->face_colors[2]); // Left
                if (x == 1.0f) glm_vec3_copy(colors[3], cubelet->face_colors[3]);  // Right
                if (y == -1.0f) glm_vec3_copy(colors[4], cubelet->face_colors[4]); // Bottom
                if (y == 1.0f) glm_vec3_copy(colors[5], cubelet->face_colors[5]);  // Top
                index++;
            }
        }
    }

    state->cube->cubeCount = CUBELET_COUNT;
    state->cube->isRotating = false;
}

void updateCubelets(State* state) {
    if (state->cube->isRotating) {
        const float rotation_speed = glm_rad(10.0f);  // Degrees per frame
        const float target_rotation = glm_rad(90.0f); // Total target rotation
        
        // Calculate incremental rotation step
        float rotation_step = fmin(rotation_speed, target_rotation - state->cube->rotation_progress);
        
        // Determine rotation direction
        float sign = state->cube->rotating_clockwise ? 1.0f : -1.0f;
        float actual_rotation = sign * rotation_step;

        // Calculate axis index (0=X, 1=Y, 2=Z) from rotation axis
        int axis_index = (state->cube->rotating_axis[0] != 0.0f) ? 0 : 
                        (state->cube->rotating_axis[1] != 0.0f) ? 1 : 2;
        
        // Determine which face we're rotating (1.0 or -1.0 along the axis)
        float face_coord = state->cube->rotating_axis[axis_index] > 0 ? 1.0f : -1.0f;

        // Update rotating angles for affected cubelets
        for (int i = 0; i < CUBELET_COUNT; i++) {
            Cubelet* cubelet = &state->cube->cubelets[i];
            // Use more precise comparison and check if cubelet is part of rotating face
            if (fabs(cubelet->position[axis_index] - face_coord) < 0.1f) {
                vec3 rotation_step_vec;
                glm_vec3_copy(state->cube->rotating_axis, rotation_step_vec);
                glm_vec3_scale(rotation_step_vec, actual_rotation, rotation_step_vec);
                glm_vec3_add(cubelet->rotating_angle, rotation_step_vec, cubelet->rotating_angle);
            }
        }

        state->cube->rotation_progress += rotation_step;

        // Finalize rotation when complete
        if (state->cube->rotation_progress >= target_rotation - 0.001f) {  // Small epsilon for floating point
            // Create rotation matrix for the final 90-degree snap
            mat4 rotation_matrix;
            glm_mat4_identity(rotation_matrix);
            float final_angle = state->cube->rotating_clockwise ? glm_rad(90.0f) : glm_rad(-90.0f);
            glm_rotate(rotation_matrix, final_angle, state->cube->rotating_axis);

            // Store original positions to avoid floating point drift
            vec3 original_positions[CUBELET_COUNT];
            for (int i = 0; i < CUBELET_COUNT; i++) {
                glm_vec3_copy(state->cube->cubelets[i].position, original_positions[i]);
            }

            for (int i = 0; i < CUBELET_COUNT; i++) {
                Cubelet* cubelet = &state->cube->cubelets[i];
                if (fabs(original_positions[i][axis_index] - face_coord) < 0.1f) {
                    // Apply rotation to position
                    vec4 pos = {original_positions[i][0], original_positions[i][1], original_positions[i][2], 1.0f};
                    glm_mat4_mulv(rotation_matrix, pos, pos);
                    
                    // Snap to exact grid positions to avoid floating point drift
                    cubelet->position[0] = roundf(pos[0]);
                    cubelet->position[1] = roundf(pos[1]); 
                    cubelet->position[2] = roundf(pos[2]);
                    
                    // Update face colors
                    rotateFaceColors(cubelet, state->cube->rotating_axis, state->cube->rotating_clockwise);
                    
                    // Reset rotation state
                    glm_vec3_zero(cubelet->rotating_angle);
                }
            }

            // End rotation
            state->cube->isRotating = false;
            state->cube->rotation_progress = 0.0f;  // Reset for next rotation
        }
    }
}

void updateCubelet(State* state) {
    if (state->cube->isRotating) {
        const float rotation_speed = glm_rad(5.0f);  // Degrees per frame
        const float target_rotation = glm_rad(90.0f); // Total target rotation
        
        // Calculate incremental rotation step
        float rotation_step = fmin(rotation_speed, target_rotation - state->cube->rotation_progress);
        
        // Determine rotation direction
        float sign = state->cube->rotating_clockwise ? 1.0f : -1.0f;
        float actual_rotation = sign * rotation_step;

        // Calculate axis index (0=X, 1=Y, 2=Z) from rotation axis
        int axis_index = (state->cube->rotating_axis[0] != 0.0f) ? 0 : 
                        (state->cube->rotating_axis[1] != 0.0f) ? 1 : 2;
        
        // Determine which face we're rotating (1.0 or -1.0 along the axis)
        float face_coord = state->cube->rotating_axis[axis_index] > 0 ? 1.0f : -1.0f;

        // Update rotating angles for affected cubelets
        for (int i = 0; i < CUBELET_COUNT; i++) {
            Cubelet* cubelet = &state->cube->cubelets[i];
            if (fabs(cubelet->position[axis_index] - face_coord) < 0.01f) {
                vec3 rotation_step_vec;
                glm_vec3_copy(state->cube->rotating_axis, rotation_step_vec);
                glm_vec3_scale(rotation_step_vec, actual_rotation, rotation_step_vec);
                glm_vec3_add(cubelet->rotating_angle, rotation_step_vec, cubelet->rotating_angle);
            }
        }

        state->cube->rotation_progress += rotation_step;

        // Finalize rotation when complete
        if (state->cube->rotation_progress >= target_rotation) {
            mat4 rotation_matrix;
            glm_mat4_identity(rotation_matrix);
            float final_angle = state->cube->rotating_clockwise ? glm_rad(90.0f) : glm_rad(-90.0f);
            glm_rotate(rotation_matrix, final_angle, state->cube->rotating_axis);

            for (int i = 0; i < CUBELET_COUNT; i++) {
                Cubelet* cubelet = &state->cube->cubelets[i];
                if (fabs(cubelet->position[axis_index] - face_coord) < 0.01f) {
                    // Update position
                    vec4 pos = {cubelet->position[0], cubelet->position[1], cubelet->position[2], 1.0f};
                    glm_mat4_mulv(rotation_matrix, pos, pos);
                    glm_vec4_copy3(pos, cubelet->position);
                    
                    // Update face colors
                    rotateFaceColors(cubelet, state->cube->rotating_axis, state->cube->rotating_clockwise);
                    
                    // Reset rotation state
                    glm_vec3_zero(cubelet->rotating_angle);
                }
            }

            // End rotation
            state->cube->isRotating = false;
        }
    }
}

void startFaceRotation(State* state, int face_index, bool clockwise) {
    if (state->cube->isRotating) return;
    
    state->cube->isRotating = true;
    state->cube->rotating_clockwise = clockwise;
    state->cube->rotation_progress = 0.0f;

    vec3 axis;
    // Set axis based on face
    switch(face_index) {
        case FACE_FRONT:    glm_vec3_copy((vec3){ 0.0f,  0.0f,  1.0f}, axis); break;
        case FACE_BACK:     glm_vec3_copy((vec3){ 0.0f,  0.0f, -1.0f}, axis); break;
        case FACE_LEFT:     glm_vec3_copy((vec3){-1.0f,  0.0f,  0.0f}, axis); break;
        case FACE_RIGHT:    glm_vec3_copy((vec3){ 1.0f,  0.0f,  0.0f}, axis); break;
        case FACE_BOTTOM:   glm_vec3_copy((vec3){ 0.0f, -1.0f,  0.0f}, axis); break;
        case FACE_TOP:      glm_vec3_copy((vec3){ 0.0f,  1.0f,  0.0f}, axis); break;
        default: state->cube->isRotating = false; return;
    }

    glm_vec3_copy(axis, state->cube->rotating_axis);

    // Calculate axis index here only for initialization
    int axis_index = (axis[0] != 0.0f) ? 0 : (axis[1] != 0.0f) ? 1 : 2;
    float face_coord = axis[axis_index] > 0 ? 1.0f : -1.0f;

    // Initialize rotating angles
    for (int i = 0; i < CUBELET_COUNT; i++) {
        Cubelet* cubelet = &state->cube->cubelets[i];
        if (fabs(cubelet->position[axis_index] - face_coord) < 0.01f) {
            glm_vec3_zero(cubelet->rotating_angle);
        } else {
            glm_vec3_zero(cubelet->rotating_angle);
        }
    }
}

void rotateFaceColors(Cubelet* c, vec3 axis, bool clockwise) {
    vec3 old[6];
    for (int i = 0; i < 6; i++) {
        glm_vec3_copy(c->face_colors[i], old[i]);
    }

    // X-axis rotation (L/R moves)
    if (fabs(axis[0]) > 0.5f) {
        if (axis[0] > 0) { // Right face
             if (clockwise) { // R
                glm_vec3_copy(old[0], c->face_colors[4]); // Front to Bottom
                glm_vec3_copy(old[4], c->face_colors[1]); // Bottom to Back
                glm_vec3_copy(old[1], c->face_colors[5]); // Back to Top
                glm_vec3_copy(old[5], c->face_colors[0]); // Top to Front
            } else { // R'
                glm_vec3_copy(old[4], c->face_colors[0]); // Bottom to Front
                glm_vec3_copy(old[0], c->face_colors[5]); // Front to Top
                glm_vec3_copy(old[5], c->face_colors[1]); // Top to Back
                glm_vec3_copy(old[1], c->face_colors[4]); // Back to Bottom
            }
        } else { // Left face
            if (clockwise) { // L
                glm_vec3_copy(old[4], c->face_colors[0]); // Bottom to Front
                glm_vec3_copy(old[0], c->face_colors[5]); // Front to Top  
                glm_vec3_copy(old[5], c->face_colors[1]); // Top to Back
                glm_vec3_copy(old[1], c->face_colors[4]); // Back to Bottom
            } else { // L'
                glm_vec3_copy(old[0], c->face_colors[4]); // Front to Bottom
                glm_vec3_copy(old[4], c->face_colors[1]); // Bottom to Back
                glm_vec3_copy(old[1], c->face_colors[5]); // Back to Top
                glm_vec3_copy(old[5], c->face_colors[0]); // Top to Front
            }
        }
    // Y-axis rotation (U/D moves)
    } else if (fabs(axis[1]) > 0.5f) {
        if (axis[1] > 0) { // Top face
            if (clockwise) { // U
                glm_vec3_copy(old[2], c->face_colors[0]); // Left to Front
                glm_vec3_copy(old[0], c->face_colors[3]); // Front to Right
                glm_vec3_copy(old[3], c->face_colors[1]); // Right to Back
                glm_vec3_copy(old[1], c->face_colors[2]); // Back to Left
            } else { // U'
                glm_vec3_copy(old[0], c->face_colors[2]); // Front to Left
                glm_vec3_copy(old[2], c->face_colors[1]); // Left to Back
                glm_vec3_copy(old[1], c->face_colors[3]); // Back to Right
                glm_vec3_copy(old[3], c->face_colors[0]); // Right to Front
            }
        } else { // Bottom face
            if (clockwise) { // D
                glm_vec3_copy(old[0], c->face_colors[2]); // Front to Left
                glm_vec3_copy(old[2], c->face_colors[1]); // Left to Back
                glm_vec3_copy(old[1], c->face_colors[3]); // Back to Right
                glm_vec3_copy(old[3], c->face_colors[0]); // Right to Front
            } else { // D'
                glm_vec3_copy(old[2], c->face_colors[0]); // Left to Front
                glm_vec3_copy(old[0], c->face_colors[3]); // Front to Right
                glm_vec3_copy(old[3], c->face_colors[1]); // Right to Back
                glm_vec3_copy(old[1], c->face_colors[2]); // Back to Left
            }
        }
    // Z-axis rotation (F/B moves)
    } else if (fabs(axis[2]) > 0.5f) {
        if (axis[2] > 0) { // Front face
            if (clockwise) { // F
                glm_vec3_copy(old[4], c->face_colors[3]); // Bottom to Right
                glm_vec3_copy(old[3], c->face_colors[5]); // Right to Top
                glm_vec3_copy(old[5], c->face_colors[2]); // Top to Left
                glm_vec3_copy(old[2], c->face_colors[4]); // Left to Bottom
            } else { // F'
                glm_vec3_copy(old[3], c->face_colors[4]); // Right to Bottom
                glm_vec3_copy(old[4], c->face_colors[2]); // Bottom to Left
                glm_vec3_copy(old[2], c->face_colors[5]); // Left to Top
                glm_vec3_copy(old[5], c->face_colors[3]); // Top to Right
            }
        } else { // Back face
            if (clockwise) { // B
                glm_vec3_copy(old[3], c->face_colors[4]); // Right to Bottom
                glm_vec3_copy(old[4], c->face_colors[2]); // Bottom to Left
                glm_vec3_copy(old[2], c->face_colors[5]); // Left to Top
                glm_vec3_copy(old[5], c->face_colors[3]); // Top to Right
            } else { // B'
                glm_vec3_copy(old[4], c->face_colors[3]); // Bottom to Right
                glm_vec3_copy(old[3], c->face_colors[5]); // Right to Top
                glm_vec3_copy(old[5], c->face_colors[2]); // Top to Left
                glm_vec3_copy(old[2], c->face_colors[4]); // Left to Bottom
            }
        }
    }
}
