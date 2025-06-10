#include "main.h"
#include "events.h"

void handleInput(State* state) {
    switch (state->event.type) {
        case SDL_QUIT:
            state->isActive = false;
            break;
        case SDL_KEYDOWN:
            switch(state->event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    state->isActive = false;
                    break;
            }
            break;
    }
}
