#include "main.h"
#include "events.h"

void handleInput(State* state) {
    bool clockwise = !(SDL_GetModState() & KMOD_SHIFT);
    switch (state->event.type) {
        case SDL_QUIT:
            state->isActive = false;
            break;
        case SDL_KEYDOWN:
            switch(state->event.key.keysym.sym) {
                case SDLK_ESCAPE: state->isActive = false; break;
                case SDLK_r: startFaceRotation(state, FACE_RIGHT, clockwise); break;
                case SDLK_l: startFaceRotation(state, FACE_LEFT, clockwise); break;
                case SDLK_u: startFaceRotation(state, FACE_TOP, clockwise); break;
                case SDLK_d: startFaceRotation(state, FACE_BOTTOM, clockwise); break;
                case SDLK_f: startFaceRotation(state, FACE_FRONT, clockwise); break;
                case SDLK_b: startFaceRotation(state, FACE_BACK, clockwise); break;
            }
            break;
    }
}
