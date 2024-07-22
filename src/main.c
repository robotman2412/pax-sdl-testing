
// SPDX-License-Identidier: MIT

#include <assert.h>
#include <pax_gfx.h>
#include <pax_gui.h>
#include <SDL.h>

// SDL window.
SDL_Window   *window;
// SDL renderer.
SDL_Renderer *renderer;
// PAX graphics context.
pax_buf_t     gfx;



pgui_grid_t root = PGUI_NEW_GRID(
    10,
    10,
    216,
    100,
    2,
    3,

    &PGUI_NEW_LABEL("This label"),
    &PGUI_NEW_TEXTBOX(),

    &PGUI_NEW_LABEL("This label"),
    &PGUI_NEW_BUTTON("This button"),

    &PGUI_NEW_LABEL("This label"),
    &PGUI_NEW_BUTTON("This button")
);

// Flush the contents of a buffer to the window.
void window_flush(SDL_Window *window, pax_buf_t *gfx) {
    static SDL_Texture *texture = NULL;
    static int          tw, th;
    SDL_Surface        *surface = SDL_GetWindowSurface(window);

    if (surface->w == pax_buf_get_width(gfx) && surface->h == pax_buf_get_height(gfx)) {
        // Direct surface update.
        SDL_LockSurface(surface);
        memcpy(surface->pixels, pax_buf_get_pixels(gfx), pax_buf_get_size(gfx));
        SDL_UnlockSurface(surface);
        SDL_UpdateWindowSurface(window);
    }
}

// Draw the graphics.
void draw();
// Update the layout.
void resized();

int main(int argc, char **argv) {
    SDL_version ver;
    SDL_GetVersion(&ver);
    printf("SDL%d.%d.%d\n", ver.major, ver.minor, ver.patch);
#ifdef SDL_HINT_VIDEODRIVER
    SDL_SetHint(SDL_HINT_VIDEODRIVER, "wayland");
#endif

    // Create the SDL contexts.
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow(
        "PAX SDL",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        800,
        480,
        SDL_WINDOW_ALLOW_HIGHDPI
    );
    if (!window) {
        return 1;
    }
    resized();

    SDL_Event event;
    while (1) {
        if (SDL_WaitEvent(&event)) {
            if (event.type == SDL_QUIT) {
                break;
            } else if (event.type == SDL_WINDOWEVENT) {
                if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
                    int width, height;
                    SDL_GetWindowSizeInPixels(window, &width, &height);
                    if (abs(width - 800) > 2 || abs(height - 480) > 2) {
                        pax_buf_destroy(&gfx);
                        resized();
                    }
                }
            } else if (event.type == SDL_KEYDOWN && !event.key.repeat) {
                if (event.key.keysym.sym == SDLK_q && (event.key.keysym.mod & KMOD_CTRL)) {
                    break;
                }
            } else if (event.type != SDL_KEYUP) {
                continue;
            }

            // Translate to PAX GUI input.
            pgui_input_t p_input;
            switch (event.key.keysym.sym) {
                default: p_input = PGUI_INPUT_NONE; break;
                case SDLK_RETURN: p_input = PGUI_INPUT_ACCEPT; break;
                case SDLK_RETURN2: p_input = PGUI_INPUT_ACCEPT; break;
                case SDLK_KP_ENTER: p_input = PGUI_INPUT_ACCEPT; break;
                case SDLK_BACKSPACE: p_input = PGUI_INPUT_BACK; break;
                case SDLK_KP_BACKSPACE: p_input = PGUI_INPUT_BACK; break;
                case SDLK_ESCAPE: p_input = PGUI_INPUT_BACK; break;
                case SDLK_UP: p_input = PGUI_INPUT_UP; break;
                case SDLK_DOWN: p_input = PGUI_INPUT_DOWN; break;
                case SDLK_LEFT: p_input = PGUI_INPUT_LEFT; break;
                case SDLK_RIGHT: p_input = PGUI_INPUT_RIGHT; break;
                case SDLK_PAGEUP: p_input = PGUI_INPUT_PGUP; break;
                case SDLK_PAGEDOWN: p_input = PGUI_INPUT_PGDN; break;
                case SDLK_HOME: p_input = PGUI_INPUT_HOME; break;
                case SDLK_END: p_input = PGUI_INPUT_END; break;
            }
            // Translate to PAX GUI event.
            pgui_event_type_t p_type;
            switch (event.type) {
                default: continue;
                case SDL_KEYDOWN: p_type = event.key.repeat ? PGUI_EVENT_TYPE_HOLD : PGUI_EVENT_TYPE_PRESS; break;
                case SDL_KEYUP: p_type = PGUI_EVENT_TYPE_RELEASE; break;
            }
            // Translate to ASCII character.
            char p_value = 0;
            if (event.key.keysym.sym && !(event.key.keysym.sym & 0xffffff00)) {
                p_value    = event.key.keysym.sym;
                bool shift = event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT);
                bool caps  = event.key.keysym.mod & KMOD_CAPS;
                if (p_value >= 'a' && p_value <= 'z') {
                    if (shift ^ caps) {
                        p_value &= 0xdf;
                    }
                } else if (shift) {
                    switch (p_value) {
                        case '`': p_value = '~'; break;
                        case '1': p_value = '!'; break;
                        case '2': p_value = '@'; break;
                        case '3': p_value = '#'; break;
                        case '4': p_value = '$'; break;
                        case '5': p_value = '%'; break;
                        case '6': p_value = '^'; break;
                        case '7': p_value = '&'; break;
                        case '8': p_value = '*'; break;
                        case '9': p_value = '('; break;
                        case '0': p_value = ')'; break;
                        case '-': p_value = '_'; break;
                        case '=': p_value = '+'; break;
                        case '[': p_value = '{'; break;
                        case ']': p_value = '}'; break;
                        case '\\': p_value = '|'; break;
                        case ';': p_value = ':'; break;
                        case '\'': p_value = '"'; break;
                        case ',': p_value = '<'; break;
                        case '.': p_value = '>'; break;
                        case '/': p_value = '?'; break;
                    }
                }
            }
            // Run the event.
            if (p_input == PGUI_INPUT_NONE && p_value == 0) {
                continue;
            }
            pgui_resp_t resp = pgui_event(
                pax_buf_get_dims(&gfx),
                (pgui_elem_t *)&root,
                NULL,
                (pgui_event_t){
                    .type    = p_type,
                    .input   = p_input,
                    .value   = p_value,
                    .modkeys = event.key.keysym.mod,
                }
            );
            if (resp) {
                if (root.base.flags & PGUI_FLAG_DIRTY) {
                    pax_background(&gfx, pgui_theme_default.bg_col);
                }
                pgui_redraw(&gfx, (pgui_elem_t *)&root, NULL);
                window_flush(window, &gfx);
            }
            if (resp == PGUI_RESP_CAPTURED_ERR) {
                printf(":c\n");
            }

        } else {
            break;
        }
    }

    return 0;
}

// Draw the graphics.
void draw() {
    pax_background(&gfx, pgui_theme_default.bg_col);
    pax_reset_2d(&gfx, PAX_RESET_ALL);
    pgui_draw(&gfx, (pgui_elem_t *)&root, NULL);
}

// Update the layout.
void resized() {
    int width, height, fake_width, fake_height;
    SDL_GetWindowSizeInPixels(window, &width, &height);
    SDL_GetWindowSize(window, &fake_width, &fake_height);
    SDL_SetWindowSize(window, 800 * fake_width / width, 480 * fake_height / height);
    SDL_GetWindowSizeInPixels(window, &width, &height);
    pax_buf_init(&gfx, NULL, width, height, PAX_BUF_32_8888ARGB);
    pgui_calc_layout(pax_buf_get_dims(&gfx), (pgui_elem_t *)&root, NULL);
    draw(&gfx);
    window_flush(window, &gfx);
}
