//
// Created by Simon on 06.02.2022.
//

#include "Input.h"

Input::Input(State *state) {
    this->state = state;
}

void Input::ProcessInput(Camera *camera, const std::function<void(Uint32)> &fp) {
    if (state->currentObject == nullptr)
        state->currentObject = camera;
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_MOUSEMOTION:
                if (rmb) {
                    state->currentObject->roll((float) event.motion.xrel, state->mouseSpeed);
                } else {
                    state->currentObject->yaw((float) event.motion.yrel, state->mouseSpeed);
                    state->currentObject->pitch((float) event.motion.xrel, state->mouseSpeed);
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_RIGHT)
                    rmb = true;
                break;
            case SDL_MOUSEBUTTONUP:
                if (event.button.button == SDL_BUTTON_RIGHT)
                    rmb = false;
                break;
            case SDL_MOUSEWHEEL:
                camera->setPerspective(camera->fov - (float) event.wheel.y * 0.1f);
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
        camera->position = glm::vec3(0.0f, 0.0f, -15.0f);
        camera->lookAt = glm::vec3(0.0f, 0.0f, 1.0f);
    }
    if (keyboardState[SDL_SCANCODE_I]) {
        fp(SDL_SCANCODE_I);
    }
    if (keyboardState[SDL_SCANCODE_O]) {
        fp(SDL_SCANCODE_O);
    }
    if (keyboardState[SDL_SCANCODE_K]) {
        fp(SDL_SCANCODE_K);
    }
    if (keyboardState[SDL_SCANCODE_L]) {
        fp(SDL_SCANCODE_L);
    }
    if (keyboardState[SDL_SCANCODE_LEFT] || keyboardState[SDL_SCANCODE_A]) {
        state->currentObject->translate(Camera::LEFT, state->speed);
    } else if (keyboardState[SDL_SCANCODE_RIGHT] || keyboardState[SDL_SCANCODE_D]) {
        state->currentObject->translate(Camera::RIGHT, state->speed);
    }
    if (keyboardState[SDL_SCANCODE_UP] || keyboardState[SDL_SCANCODE_W]) {
        state->currentObject->translate(Camera::FORWARD, state->speed);
    } else if (keyboardState[SDL_SCANCODE_DOWN] || keyboardState[SDL_SCANCODE_S]) {
        state->currentObject->translate(Camera::BACKWARD, state->speed);
    }
}
