
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


// Button press callback.
void on_press(pgui_button_t *button, void *cookie) {
    printf("%s pressed\n", button->text);
}

// Dropdown options.
char const *options[] = {
    "Thing",
    "Another",
    "Third",
    "Fourth",
    "Fifth",
    "Sixth",
};

// Children of root object.
pgui_base_t *root_children[] = {
    (pgui_base_t *) &(pgui_label_t){
        .base = {
            .type  = PGUI_TYPE_LABEL,
            .flags = PGUI_FLAG_FILLCELL,
        },
        .text  = "label 1",
        .align = PAX_ALIGN_LEFT,
    },
    (pgui_base_t *) &(pgui_button_t){
        .base = {
            .type  = PGUI_TYPE_BUTTON,
            .flags = PGUI_FLAG_FILLCELL,
        },
        .text = "button 🅰",
        .callback = on_press,
    },
    
    (pgui_base_t *) &(pgui_label_t){
        .base = {
            .type  = PGUI_TYPE_LABEL,
            .flags = PGUI_FLAG_FILLCELL,
        },
        .text  = "label 2",
        .align = PAX_ALIGN_LEFT,
    },
    (pgui_base_t *) &(pgui_button_t){
        .base = {
            .type  = PGUI_TYPE_BUTTON,
            .flags = PGUI_FLAG_FILLCELL,
        },
        .text = "button 2",
        .callback = on_press,
    },
    
    (pgui_base_t *) &(pgui_label_t){
        .base = {
            .type  = PGUI_TYPE_LABEL,
            .flags = PGUI_FLAG_FILLCELL,
        },
        .text  = "label 3",
        .align = PAX_ALIGN_LEFT,
    },
    (pgui_base_t *) &(pgui_dropdown_t){
        .base = {
            .type  = PGUI_TYPE_DROPDOWN,
            .flags = PGUI_FLAG_FILLCELL,
        },
        .options_len = sizeof(options) / sizeof(char const *),
        .options = options,
    },
};

// GUI root.
pgui_grid_t root = {
    .box = {
        .base = {
            .type  = PGUI_TYPE_GRID,
            .pos   = {10, 10},
            .flags = PGUI_FLAG_HIGHLIGHT | PGUI_FLAG_INACTIVE,
        },
        .selected     = -1,
        .children_len = sizeof(root_children) / sizeof(pgui_base_t *),
        .children     = root_children,
    },
    .cell_size = {110, 38},
    .cells     = {2, 3},
};

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
void draw(pax_buf_t *gfx);
// Update the layout.
void resized(pax_buf_t *gfx);

int main(int argc, char **argv) {
    // Create the SDL contexts.
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow(
        "PAX SDL",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        400,
        300,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI
    );
    if (!window) {
        return 1;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        return 1;
    }
    // int res
    //     = SDL_CreateWindowAndRenderer(400, 300, SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI, &window, &renderer);
    // SDL_SetWindowTitle(window, "PAX SDL");

    {
        int width, height;
        SDL_GetRendererOutputSize(renderer, &width, &height);
        pax_buf_init(&gfx, NULL, width, height, PAX_BUF_32_8888ARGB);
    }
    resized(&gfx);
    draw(&gfx);
    window_flush(window, &gfx);

    bool      lctrl = false;
    SDL_Event event;
    while (1) {
        if (SDL_WaitEvent(&event)) {
            if (event.type == SDL_QUIT)
                break;
            if (event.type == SDL_WINDOWEVENT) {
                if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                    pax_buf_destroy(&gfx);
                    int width, height;
                    SDL_GetRendererOutputSize(renderer, &width, &height);
                    pax_buf_init(&gfx, NULL, width, height, PAX_BUF_32_8888ARGB);
                    resized(&gfx);
                    draw(&gfx);
                    window_flush(window, &gfx);
                }
            }
            if (event.type == SDL_KEYDOWN && !event.key.repeat) {
                if (event.key.keysym.sym == SDLK_LCTRL)
                    lctrl = true;
                if (event.key.keysym.sym == SDLK_q && lctrl)
                    break;
            }
            if (event.type == SDL_KEYUP) {
                if (event.key.keysym.sym == SDLK_LCTRL)
                    lctrl = false;
            }

            // Translate to PAX GUI event.
            pgui_input_t p_input;
            switch (event.key.keysym.sym) {
                default: continue;
                case SDLK_RETURN: p_input = PGUI_INPUT_ACCEPT; break;
                case SDLK_RETURN2: p_input = PGUI_INPUT_ACCEPT; break;
                case SDLK_KP_ENTER: p_input = PGUI_INPUT_ACCEPT; break;
                case SDLK_a: p_input = PGUI_INPUT_ACCEPT; break;
                case SDLK_BACKSPACE: p_input = PGUI_INPUT_BACK; break;
                case SDLK_KP_BACKSPACE: p_input = PGUI_INPUT_BACK; break;
                case SDLK_ESCAPE: p_input = PGUI_INPUT_BACK; break;
                case SDLK_b: p_input = PGUI_INPUT_BACK; break;
                case SDLK_UP: p_input = PGUI_INPUT_UP; break;
                case SDLK_DOWN: p_input = PGUI_INPUT_DOWN; break;
                case SDLK_LEFT: p_input = PGUI_INPUT_LEFT; break;
                case SDLK_RIGHT: p_input = PGUI_INPUT_RIGHT; break;
            }
            pgui_event_t p_event;
            switch (event.type) {
                default: continue;
                case SDL_KEYDOWN: p_event = event.key.repeat ? PGUI_EVENT_HOLD : PGUI_EVENT_PRESS; break;
                case SDL_KEYUP: p_event = PGUI_EVENT_RELEASE; break;
            }
            pgui_resp_t resp = pgui_event(&root.base, p_input, p_event);
            if (resp) {
                if (root.base.flags & PGUI_FLAG_DIRTY) {
                    pax_background(&gfx, pgui_theme_default.bg_col);
                }
                pgui_redraw(&gfx, &root.base, NULL);
                window_flush(window, &gfx);
            }

        } else {
            break;
        }
    }

    return 0;
}

// Draw the graphics.
void draw(pax_buf_t *gfx) {
    pax_background(gfx, pgui_theme_default.bg_col);
    pax_reset_2d(gfx, PAX_RESET_ALL);
    pgui_draw(gfx, &root.base, NULL);
}

// Update the layout.
void resized(pax_buf_t *gfx) {
    root.base.size.x = pax_buf_get_width(gfx) - 20;
    root.base.size.y = pax_buf_get_height(gfx) - 20;
    pgui_calc_layout(&root.base, NULL);
}
