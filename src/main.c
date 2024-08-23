#include "8080.h"
#include "machine.h"
#include "raylib.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define RAYGUI_IMPLEMENTATION

#include "raygui.h"

#define CLOCK_SPEED (2000000)

// number of cycles taken for each opcode from 0x00 to 0xff
static const uint8_t cycles_lookup[256] = {
    //  0  1   2   3   4   5   6   7   8  9   A   B   C   D   E  F
    4, 10, 7, 5, 5, 5, 7, 4, 4, 10, 7, 5, 5, 5, 7, 4,           // 0
    4, 10, 7, 5, 5, 5, 7, 4, 4, 10, 7, 5, 5, 5, 7, 4,           // 1
    4, 10, 16, 5, 5, 5, 7, 4, 4, 10, 16, 5, 5, 5, 7, 4,         // 2
    4, 10, 13, 5, 10, 10, 10, 4, 4, 10, 13, 5, 5, 5, 7, 4,      // 3
    5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5,             // 4
    5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5,             // 5
    5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5,             // 6
    7, 7, 7, 7, 7, 7, 7, 7, 5, 5, 5, 5, 5, 5, 7, 5,             // 7
    4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,             // 8
    4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,             // 9
    4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,             // A
    4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,             // B
    5, 10, 10, 10, 11, 11, 7, 11, 5, 10, 10, 10, 11, 17, 7, 11, // C
    5, 10, 10, 10, 11, 11, 7, 11, 5, 10, 10, 10, 11, 17, 7, 11, // D
    5, 10, 10, 18, 11, 11, 7, 11, 5, 5, 10, 4, 11, 17, 7, 11,   // E
    5, 10, 10, 4, 11, 11, 7, 11, 5, 5, 10, 4, 11, 17, 7, 11     // F
};

static void GenerateInterrupt(State8080 *state, int interrupt_number) {
    state->int_pending = 1;
    state->int_number = interrupt_number;
}

static void execute_interrupt(State8080 *state, int interrupt_number) {
    state->memory[state->sp - 1] = (state->pc >> 8) & 0xff;
    state->memory[state->sp - 2] = state->pc & 0xff;
    state->sp -= 2;

    /* state->pc = 8 * interrupt_number; */
    switch (interrupt_number) {
    case 1:
        state->pc = 0x08;
        break;
    case 2:
        state->pc = 0x10;
        break;
    default:
        printf("Interrupt %d not implemented\n", interrupt_number);
        exit(1);
    }
}

static void copy_screen_buffer_grayscale(uint8_t *screen_buffer, uint8_t *memory) {
    int index;
    for (int i = 0; i < 256 * 224 / 8; i++) {
        uint8_t byte = memory[i];
        for (int j = 0; j < 8; j++) {
            uint8_t bit = (byte >> j) % 2;

            index = 3 * 8 * i + 3 * j;
            screen_buffer[index] = 0xff * bit;
            screen_buffer[index + 1] = 0xff * bit;
            screen_buffer[index + 2] = 0xff * bit;
        }
    }
}

static void copy_screen_buffer_colour(uint8_t *screen_buffer, uint8_t *memory) {
    int index;
    int row; // out of 256
    int col; // out of 224
    uint8_t r;
    uint8_t g;
    uint8_t b;
    for (int i = 0; i < 256 * 224 / 8; i++) {
        uint8_t byte = memory[i];
        for (int j = 0; j < 8; j++) {
            uint8_t bit = (byte >> j) % 2;

            index = 3 * 8 * i + 3 * j;
            col = 255 - (8 * i + j) % 256;
            row = (8 * i + j) / 224;

            if (32 <= col && col < 64) {
                r = 0xff;
                g = 0x00;
                b = 0x00;
            } else if (184 <= col && col < 240) {
                r = 0x00;
                g = 0xff;
                b = 0x00;
            } else if (col >= 240 && row >= 16 && row < 134) {
                r = 0x00;
                g = 0xff;
                b = 0x00;
            } else {
                r = 0xff;
                g = 0xff;
                b = 0xff;
            }
            screen_buffer[index] = r * bit;
            screen_buffer[index + 1] = g * bit;
            screen_buffer[index + 2] = b * bit;
        }
    }
}

static void colorize_screen_buffer(uint8_t *screen_buffer) {
    int index;
    for (int row = 0; row < 224; row++) {
        for (int col = 32; col < 64; col++) {
            index = 256 * row + 255 - col;
            index *= 3;
            screen_buffer[index + 1] = 0x00;
            screen_buffer[index + 2] = 0x00;
        }
        for (int col = 184; col < 240; col++) {
            index = 256 * row + 255 - col;
            index *= 3;
            screen_buffer[index] = 0x00;
            screen_buffer[index + 2] = 0x00;
        }
    }
    for (int row = 16; row < 134; row++) {
        for (int col = 240; col < 256; col++) {
            index = 256 * row + 255 - col;
            index *= 3;
            screen_buffer[index] = 0x00;
            screen_buffer[index + 2] = 0x00;
        }
    }
}

static void decolorize_screen_buffer(uint8_t *screen_buffer) {
    int index;
    uint8_t colour;
    for (int row = 0; row < 224; row++) {
        for (int col = 32; col < 64; col++) {
            index = 256 * row + 255 - col;
            index *= 3;
            colour = screen_buffer[index];
            screen_buffer[index + 1] = colour;
            screen_buffer[index + 2] = colour;
        }
        for (int col = 184; col < 240; col++) {
            index = 256 * row + 255 - col;
            index *= 3;
            colour = screen_buffer[index + 1];
            screen_buffer[index] = colour;
            screen_buffer[index + 2] = colour;
        }
    }
    for (int row = 16; row < 134; row++) {
        for (int col = 240; col < 256; col++) {
            index = 256 * row + 255 - col;
            index *= 3;
            colour = screen_buffer[index + 1];
            screen_buffer[index] = colour;
            screen_buffer[index + 2] = colour;
        }
    }
}

static void advance_emulation(double dt, SpaceInvadersMachine *machine) {
    State8080 *state = machine->state;
    uint8_t *opcode;
    int cycle_count = 0;

    int elapsed_cycles = floor(dt * (double)CLOCK_SPEED);
    while (cycle_count < elapsed_cycles) {
        opcode = &state->memory[state->pc];
        cycle_count += cycles_lookup[opcode[0]];

        if (state->int_pending && state->int_enable && state->int_delay == 0) {
            state->int_pending = 0;
            state->int_enable = 0;
            execute_interrupt(state, state->int_number);
        } else if (*opcode == IN) {
            uint8_t port = opcode[1];
            EmulateMachineIn(machine, port);
        } else if (*opcode == OUT) {
            uint8_t port = opcode[1];
            EmulateMachineOut(machine, port);
        } else {
            Emulate8080Op(state);
        }
    }
}

void CustomGuiTabBar(Rectangle bounds, const char **text, int count, int *active);

int main() {

    int base_scale = 4;
    int scale = base_scale;
    int game_window_width = 224 * scale;
    int game_window_height = 256 * scale;
    int window_width = game_window_height + 100 * scale;
    int window_height = game_window_height;

    SetTraceLogLevel(LOG_WARNING);
    InitWindow(window_width, window_height, "zoctante - space invaders");

    GuiSetStyle(DEFAULT, TEXT_SIZE, scale * 7);
    GuiSetStyle(VALUEBOX, TEXT_PADDING, 10);
    GuiSetStyle(VALUEBOX, BORDER_WIDTH, 0);
    GuiSetStyle(CHECKBOX, TEXT_PADDING, 16);

    int monitor = 0;
    Vector2 monitor_pos = GetMonitorPosition(monitor);
    int monitor_height = GetMonitorHeight(monitor);
    int monitor_width = GetMonitorWidth(monitor);

    SetWindowMonitor(monitor);

    int window_x = (int)monitor_pos.x + (monitor_width - window_width) / 2;
    int window_y = (int)monitor_pos.y + (monitor_height - window_height) / 2;
    SetWindowPosition(window_x, window_y);
    InitAudioDevice();

    SpaceInvadersMachine *machine = init_machine();
    State8080 *state = machine->state;
    read_rom_into_memory(state->memory, "./roms/invaders.concatenated", 0x0000);
    uint8_t *state8080_memory = state->memory;

    unsigned char *opcode;

    long iteration_number = 0;

    bool colour_display = true;

    // 2400-3FFF 7K video RAM
    uint8_t *screen_buffer = malloc(3 * 8 * 256 * 224);
    copy_screen_buffer_grayscale(screen_buffer, &state->memory[0x2400]);
    if (colour_display) {
        colorize_screen_buffer(screen_buffer);
    }

    // The raster resolution is 256x224 at 60Hz
    // The monitor is rotated in the cabinet 90 degrees counter-clockwise
    Image image = {
        .format = (int)PIXELFORMAT_UNCOMPRESSED_R8G8B8,
        .mipmaps = 1,
        .width = 256,
        .height = 224,
        .data = screen_buffer,
    };

    Texture2D texture = LoadTextureFromImage(image);

    float speed = 1.0;

    double time;
    double time_since_last_interrupt;
    double interrupt_delay = 1.0 / (speed * 120.0); // delay between two interrupts
    double last_interrupt_time;                     // time of the last interrupt
    state->which_interrupt = 1;

    double time_since_last_draw;
    double frame_dt = 1.0 / 60.0;

    double dt = 1.0 / 120.0; // elapsed in-game time between two interrupts

    // initialisation
    // advance emulation until the first interrupt
    // it's always the same amount of in game time dt
    load_high_score(state);
    advance_emulation(dt, machine);
    time = GetTime();
    last_interrupt_time = time - interrupt_delay;
    time_since_last_draw = frame_dt;

    int tilt_bounce = 0; // in frames

    // Gui variables
    bool edit_scale = false;
    bool paused = true;
    bool display_fps = false;
    bool display_settings = true;
    bool weird_tilt = true;

    int new_scale = scale;

    const int nb_tabs = 3;
    const char *tab_text[3] = {"settings", "controls", "debug"};
    int active_tab = 1;

    while (!WindowShouldClose()) {

        if (!paused) {
            time = GetTime();
            time_since_last_interrupt = time - last_interrupt_time;

            // Generate 2 screen interrupts per in game frame
            if (time_since_last_interrupt > interrupt_delay) {

                last_interrupt_time = GetTime();

                // The system gets RST 1 when the beam is *near* the middle of the
                // screen and RST 2 when it is at the end (start of VBLANK).
                GenerateInterrupt(state, state->which_interrupt);

                // Update the game screen on RST 2 interrupt
                // NOTE: For now, that means that higher speed means higher framerate
                if (state->which_interrupt == 2) {

                    // 2400-3FFF 7K Video RAM
                    copy_screen_buffer_grayscale(screen_buffer, &state8080_memory[0x2400]);
                    if (colour_display) {
                        colorize_screen_buffer(screen_buffer);
                    }

                    UpdateTexture(texture, screen_buffer);
                }

                // Handle input for the next frame
                emulate_machines_input(machine);

                if (IsKeyDown(KEY_T)) {
                    tilt_bounce = 660;
                } else {
                    if (tilt_bounce > 0) {
                        tilt_bounce--;
                    }
                }

                // alternate between 1 and 2
                state->which_interrupt = (state->which_interrupt == 1) ? 2 : 1;

                // advance simulation for next interrupt
                // it's always the same amount of in game time dt
                advance_emulation(dt, machine);
            }
        }

        // When the game plays, the display is driven by update_screen
        // When the game is paused, we need to manually keep track of the elapsed time to update the gui
        if (GetTime() - time_since_last_draw > frame_dt) {
            time_since_last_draw = GetTime();

            if (IsKeyPressed(KEY_TAB) && !display_settings) {
                display_settings = true;
                ShowCursor();
                SetWindowSize(window_width, window_height);
            } else if (IsKeyPressed(KEY_TAB) && display_settings) {
                display_settings = false;
                SetWindowSize(game_window_width, game_window_height);
                HideCursor();
            }

            paused = display_settings;

            BeginDrawing();
            ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

            Color tint;
            if (display_settings) {
                tint = GRAY;
            } else {
                tint = WHITE;
            }

            if (weird_tilt && tilt_bounce != 0 && !display_settings) {
                DrawTexturePro(texture, (Rectangle){0, 0, 256, 224}, (Rectangle){0, 0, scale * 256, scale * 224}, (Vector2){0, 0}, 10, tint);
            } else {
                DrawTexturePro(texture, (Rectangle){0, 0, 256, 224}, (Rectangle){0, 0, scale * 256, scale * 224}, (Vector2){scale * 256, 0}, -90, tint);
            }

            if (display_fps) {
                DrawFPS(10, 10);
            }

            if (display_settings) {
                Rectangle tab_bounds = {
                    224 * base_scale + 6 * base_scale,
                    6 * base_scale,
                    100 * base_scale,
                    10 * base_scale,
                };

                CustomGuiTabBar(tab_bounds, &tab_text[0], nb_tabs, &active_tab);

                if (active_tab == 0) {

                    Rectangle scale_box = {
                        224 * base_scale + 6 * base_scale + (float)MeasureText("scale =", GuiGetStyle(DEFAULT, TEXT_SIZE)),
                        6 * base_scale + 16 * base_scale,
                        10 * base_scale,
                        10 * base_scale,
                    };
                    if (GuiValueBox(scale_box, "scale =", &new_scale, 1, 4, edit_scale)) {
                        edit_scale = !edit_scale;
                    } else {
                        if (!edit_scale && new_scale != scale) {
                            scale = new_scale;
                            game_window_width = 224 * scale;
                            game_window_height = 256 * scale;
                        }
                    }

                    Rectangle speed_slider_box = {
                        224 * base_scale + 6 * base_scale + (float)MeasureText("speed ", GuiGetStyle(DEFAULT, TEXT_SIZE)) - (float)GuiGetStyle(SLIDER, TEXT_PADDING),
                        6 * base_scale + 2 * 16 * base_scale,
                        90 * base_scale,
                        10 * base_scale,
                    };
                    GuiSlider(speed_slider_box, "speed ", TextFormat("%0.2f", speed), &speed, 1.0, 10.0);
                    /* if (guiSliderDragging) { */
                    /*     paused = true; */
                    /* } */

                    // update variables using speed
                    interrupt_delay = 1.0 / (speed * 120.0);

                    Rectangle colour_checkbox_box = {
                        224 * base_scale + 6 * base_scale,
                        6 * base_scale + 3 * 16 * base_scale + 16,
                        6 * base_scale,
                        6 * base_scale,
                    };

                    bool colour_temp = colour_display;
                    GuiCheckBox(colour_checkbox_box, "colour", &colour_display);
                    if (colour_temp != colour_display) {
                        if (colour_display) {
                            colorize_screen_buffer(screen_buffer);
                            UpdateTexture(texture, screen_buffer);
                        } else {
                            decolorize_screen_buffer(screen_buffer);
                            UpdateTexture(texture, screen_buffer);
                        }
                    }

                    Rectangle tilt_checkbox_box = {
                        224 * base_scale + 6 * base_scale,
                        6 * base_scale + 4 * 16 * base_scale + 16,
                        6 * base_scale,
                        6 * base_scale,
                    };
                    GuiCheckBox(tilt_checkbox_box, "tilt", &weird_tilt);

                } else if (active_tab == 1) {

                    int nb_controls = 9;
                    char *controls_text[9] = {
                        "tab = play/pause",
                        "esc = quit",
                        "c = coin",
                        "1 = start a game in one-player mode",
                        "2 = start a game in two-player mode",
                        "left = move the player left",
                        "right = move the player right",
                        "space = shoot",
                        "t = tilt",
                    };
                    Rectangle control_box = {
                        224 * base_scale + 6 * base_scale,
                        0.5 * GuiGetStyle(DEFAULT, TEXT_SIZE) + 6 * base_scale + 16 * base_scale,
                        220 * base_scale,
                        0,
                    };

                    for (int i = 0; i < nb_controls; i++) {
                        GuiLabel(control_box, controls_text[i]);
                        control_box.y += 16 * base_scale;
                    }
                } else if (active_tab == 2) {
                    Rectangle fps_checkbox_box = {
                        224 * base_scale + 6 * base_scale,
                        6 * base_scale + 16 * base_scale,
                        6 * base_scale,
                        6 * base_scale,
                    };
                    GuiCheckBox(fps_checkbox_box, "display fps ", &display_fps);
                }
            }

            EndDrawing();
        }
    }

    save_high_score(state);

    UnloadTexture(texture);
    free_machine(machine);
    free(screen_buffer);

    CloseAudioDevice();
    CloseWindow();
    return 0;
}

void CustomGuiTabBar(Rectangle bounds, const char **text, int count, int *active) {
    /* #define RAYGUI_TABBAR_ITEM_WIDTH 160 */

    int result = -1;
    // GuiState state = guiState;

    Rectangle tabBounds = {bounds.x, bounds.y, 160, bounds.height};

    if (*active < 0)
        *active = 0;
    else if (*active > count - 1)
        *active = count - 1;

    int offsetX = 0; // Required in case tabs go out of screen
    offsetX = (*active + 2) * RAYGUI_TABBAR_ITEM_WIDTH - GetScreenWidth();
    if (offsetX < 0)
        offsetX = 0;

    bool toggle = false; // Required for individual toggles

    // Draw control
    //--------------------------------------------------------------------
    for (int i = 0; i < count; i++) {
        tabBounds.x = bounds.x + (RAYGUI_TABBAR_ITEM_WIDTH + 4) * i - offsetX;

        if (tabBounds.x < GetScreenWidth()) {
            // Draw tabs as toggle controls
            int textAlignment = GuiGetStyle(TOGGLE, TEXT_ALIGNMENT);
            int textPadding = GuiGetStyle(TOGGLE, TEXT_PADDING);
            int textSize = GuiGetStyle(DEFAULT, TEXT_SIZE);
            GuiSetStyle(TOGGLE, TEXT_ALIGNMENT, TEXT_ALIGN_MIDDLE);
            GuiSetStyle(TOGGLE, TEXT_PADDING, 0);
            GuiSetStyle(DEFAULT, TEXT_SIZE, textSize + 2);

            if (i == (*active)) {
                toggle = true;
                GuiToggle(tabBounds, text[i], &toggle);
            } else {
                toggle = false;
                /* GuiToggle(tabBounds, GuiIconText(ICON_NONE, text[i]), &toggle); */
                GuiToggle(tabBounds, text[i], &toggle);
                if (toggle)
                    *active = i;
            }

            GuiSetStyle(TOGGLE, TEXT_PADDING, textPadding);
            GuiSetStyle(TOGGLE, TEXT_ALIGNMENT, textAlignment);
            GuiSetStyle(DEFAULT, TEXT_SIZE, textSize);
        }
    }

    // Draw tab-bar bottom line
    GuiDrawRectangle(RAYGUI_CLITERAL(Rectangle){bounds.x, bounds.y + bounds.height - 1, bounds.width, 1}, 0, BLANK, GetColor(GuiGetStyle(TOGGLE, BORDER_COLOR_NORMAL)));
    //--------------------------------------------------------------------
}
