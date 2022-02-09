//
// Created by Simon on 06.02.2022.
//

#include "Input.h"

Input::Input(State *state) {
    this->state = state;
}

void Input::ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_MOUSEMOTION:
                state->camera.rotation.x -= state->mouseSpeed * (float) event.motion.xrel;
                state->camera.rotation.y += state->mouseSpeed * (float) event.motion.yrel;
                break;
            case SDL_MOUSEWHEEL:
                state->fov -= (float) event.wheel.y * 0.1f;
                state->projectionMatrix = glm::perspective(state->fov, state->aspect, 0.1f, 100.0f);
                break;
            case SDL_QUIT:
                state->state = GState::STOPPED;
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_b)
                    state->debug = !state->debug;
                if (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_RETURN2 ||
                    event.key.keysym.sym == SDLK_ESCAPE)
                    state->state = GState::STOPPED;
                break;
            default:
                break;
        }
    }

    const Uint8 *keyboardState = SDL_GetKeyboardState(nullptr);

    if (keyboardState[SDL_SCANCODE_KP_MINUS]) {
        state->delay++;
        state->speed++;
    } else if (keyboardState[SDL_SCANCODE_KP_PLUS]) {
        if (state->delay != 0) {
            state->delay--;
            state->speed--;
        }

    }
    if (keyboardState[SDL_SCANCODE_P]) {
        state->camera.position = glm::vec3(0.0f, 0.0f, 15.0f);
        state->camera.rotation = glm::vec2(0.0f, 0.0f);
    }
    if (keyboardState[SDL_SCANCODE_LEFT] || keyboardState[SDL_SCANCODE_A]) {
        state->camera.position += state->camera.right * state->speed;
    } else if (keyboardState[SDL_SCANCODE_RIGHT] || keyboardState[SDL_SCANCODE_D]) {
        state->camera.position -= state->camera.right * state->speed;
    }
    if (keyboardState[SDL_SCANCODE_UP] || keyboardState[SDL_SCANCODE_W]) {
        state->camera.position -= state->camera.direction * state->speed;
    } else if (keyboardState[SDL_SCANCODE_DOWN] || keyboardState[SDL_SCANCODE_S]) {
        state->camera.position += state->camera.direction * state->speed;
    }

    state->camera.direction = glm::vec3(sin(state->camera.rotation.x) * cos(state->camera.rotation.y),
                                        sin(state->camera.rotation.y),
                                        cos(state->camera.rotation.x) * cos(state->camera.rotation.y));

    state->camera.right = glm::vec3(sin(state->camera.rotation.x - glm::half_pi<float>()),
                                    0.0,
                                    cos(state->camera.rotation.x - glm::half_pi<float>()));

    state->camera.up = glm::cross(state->camera.right, state->camera.direction);
    state->viewMatrix = glm::lookAt(state->camera.position, state->camera.position - state->camera.direction,
                                    state->camera.up);
}
