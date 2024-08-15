#include "8080.h"
#include "machine.h"
#include "raylib.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

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

static void copy_screen_buffer_grayscale(uint8_t *screen_buffer,
                                         uint8_t *memory, int scale) {
    int index;
    int new_index;
    int new_row;
    int new_col;
    for (int i = 0; i < 256 * 224 / 8; i++) {
        uint8_t byte = memory[i];
        for (int xs = 0; xs < scale; xs++) {
            for (int ys = 0; ys < scale; ys++) {
                for (int j = 0; j < 8; j++) {
                    uint8_t bit = (byte >> j) % 2;

                    index = 8 * i + j;
                    new_row = 256 - index % 256;
                    new_col = index / 256;
                    new_index = new_row * 224 * scale * scale + new_col * scale;
                    new_index += xs;
                    new_index += scale * 224 * ys;

                    screen_buffer[new_index] = 0xff * bit;
                }
            }
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

int main() {

    int scale = 4;
    int width = 224 * scale;
    int height = 256 * scale;

    SetTraceLogLevel(LOG_WARNING);
    InitWindow(width, height, "zoctante - space invaders");
    SetWindowMonitor(1);
    SetWindowPosition(10, 10);
    InitAudioDevice();
    SetTargetFPS(60);

    SpaceInvadersMachine *machine = init_machine();
    State8080 *state = machine->state;
    read_rom_into_memory(state->memory, "./roms/invaders.concatenated", 0x0000);
    uint8_t *state8080_memory = state->memory;

    unsigned char *opcode;

    long iteration_number = 0;

    // 2400-3FFF 7K video RAM
    uint8_t *screen_buffer = malloc(scale * 8 * 256 * 224);
    copy_screen_buffer_grayscale(screen_buffer, &state->memory[0x2400], scale);

    // The raster resolution is 256x224 at 60Hz
    // The monitor is rotated in the cabinet 90 degrees counter-clockwise
    Image image = {
        .format = (int)PIXELFORMAT_UNCOMPRESSED_GRAYSCALE,
        .mipmaps = 1,
        .width = scale * 224,
        .height = scale * 256,
        .data = screen_buffer,
    };

    Texture2D texture = LoadTextureFromImage(image);

    double time;
    double time_since_last_interrupt;
    double interrupt_delay = 1.0 / 120.0; // delay between two interrupts
    double last_interrupt_time;           // time of the last interrupt
    state->which_interrupt = 1;

    double dt;
    double last_cycle_time = time;
    int elapsed_cycles;
    int cycle_count;
    int cycles_per_frame = 0;

    // init
    // advance emulation until the first interrupt
    dt = interrupt_delay;
    advance_emulation(dt, machine);
    time = GetTime();
    last_interrupt_time = time - interrupt_delay;

    while (!WindowShouldClose()) {

        time = GetTime();
        time_since_last_interrupt = time - last_interrupt_time;

        // Generate 2 screen interrupts per frame (60Hz so every 1/120s)
        if (time_since_last_interrupt > interrupt_delay) {

            // The system gets RST 1 when the beam is *near* the middle of the
            // screen and RST 2 when it is at the end (start of VBLANK).
            GenerateInterrupt(state, state->which_interrupt);
            last_interrupt_time = GetTime();

            // Draw on RST 2 interrupt
            if (state->which_interrupt == 2) {
                // 2400-3FFF 7K Video RAM
                copy_screen_buffer_grayscale(screen_buffer, &state8080_memory[0x2400],
                                             scale);

                UpdateTexture(texture, image.data);

                BeginDrawing();
                ClearBackground(BLACK);
                DrawTexture(texture, (width - 224 * scale) / 2,
                            (height - 256 * scale) / 2, WHITE);
                DrawFPS(10, 10);
                EndDrawing();

                // Handle input for the next frame
                emulate_machines_input(machine);
            }

            // alternate between 1 and 2
            state->which_interrupt = (state->which_interrupt == 1) ? 2 : 1;

            // advance simulation for next interrupt
            dt = interrupt_delay;
            advance_emulation(dt, machine);
            last_cycle_time = GetTime();
        }
    }

    UnloadTexture(texture);
    free_machine(machine);
    free(screen_buffer);

    CloseAudioDevice();
    CloseWindow();
    return 0;
}
