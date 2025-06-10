#include "main.h"
#include "events.h"

void handleInput(State* state) {
    switch (state->event.type) {
        case SDL_QUIT:
            state->isActive = false;
            break;
    }
}
