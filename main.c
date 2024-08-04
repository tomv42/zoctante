#include "./raylib/include/raylib.h"
#include "8080.h"
#include "comparison.h"
#include "i8080.h"
#include "machine.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define FRAME_RATE (60)
#define CLOCK_SPEED (2000000)

// number of cycles taken for each opcode from 0x00 to 0xff
static const uint8_t cycles_lookup[256] = {
    //  0  1   2   3   4   5   6   7   8  9   A   B   C   D   E  F
    4, 10, 7,  5,  5,  5,  7,  4,  4, 10, 7,  5,  5,  5,  7, 4,  // 0
    4, 10, 7,  5,  5,  5,  7,  4,  4, 10, 7,  5,  5,  5,  7, 4,  // 1
    4, 10, 16, 5,  5,  5,  7,  4,  4, 10, 16, 5,  5,  5,  7, 4,  // 2
    4, 10, 13, 5,  10, 10, 10, 4,  4, 10, 13, 5,  5,  5,  7, 4,  // 3
    5, 5,  5,  5,  5,  5,  7,  5,  5, 5,  5,  5,  5,  5,  7, 5,  // 4
    5, 5,  5,  5,  5,  5,  7,  5,  5, 5,  5,  5,  5,  5,  7, 5,  // 5
    5, 5,  5,  5,  5,  5,  7,  5,  5, 5,  5,  5,  5,  5,  7, 5,  // 6
    7, 7,  7,  7,  7,  7,  7,  7,  5, 5,  5,  5,  5,  5,  7, 5,  // 7
    4, 4,  4,  4,  4,  4,  7,  4,  4, 4,  4,  4,  4,  4,  7, 4,  // 8
    4, 4,  4,  4,  4,  4,  7,  4,  4, 4,  4,  4,  4,  4,  7, 4,  // 9
    4, 4,  4,  4,  4,  4,  7,  4,  4, 4,  4,  4,  4,  4,  7, 4,  // A
    4, 4,  4,  4,  4,  4,  7,  4,  4, 4,  4,  4,  4,  4,  7, 4,  // B
    5, 10, 10, 10, 11, 11, 7,  11, 5, 10, 10, 10, 11, 17, 7, 11, // C
    5, 10, 10, 10, 11, 11, 7,  11, 5, 10, 10, 10, 11, 17, 7, 11, // D
    5, 10, 10, 18, 11, 11, 7,  11, 5, 5,  10, 4,  11, 17, 7, 11, // E
    5, 10, 10, 4,  11, 11, 7,  11, 5, 5,  10, 4,  11, 17, 7, 11  // F
};

static void GenerateInterrupt(State8080 *state, int interrupt_number) {
    if (state->int_enable) {

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
        }

        //"DI"
        state->int_enable = 0;
    }
}

static void copy_screen_buffer_grayscale(uint8_t *screen_buffer, uint8_t *memory) {
    for (int i = 0; i < 256 * 224 / 8; i++) {
        uint8_t byte = memory[i];
        for (int j = 0; j < 8; j++) {
            uint8_t bit = (byte >> j) % 2;
            screen_buffer[8 * i + j] = 0xff * bit;
        }
    }
}

int main() {

    SpaceInvadersMachine *machine = init_machine();
    State8080 *state = machine->state;
    read_rom_into_memory(state, "invaders.concatenated", 0x0000);

    i8080 *c = init_benchmark_emulator("invaders.concatenated", 0x0000);

    unsigned char *opcode;

    long iteration_number = 0;
    long max_iter = 60000;

    uint8_t *i8080_memory = get_benchmark_memory();
    uint8_t *state8080_memory = state->memory;

    int scale = 4;
    int width = 224 * scale;
    int height = 256 * scale;

    InitWindow(width, height, "zoctante - space invaders");
    SetWindowMonitor(0);
    SetWindowPosition(10, 10);

    // 2400-3FFF 7K video RAM
    /* uint8_t *screen_buffer = &state->memory[0x2400]; */
    /* uint8_t *screen_buffer = memory + 0x2400; */
    uint8_t *screen_buffer = malloc(8 * 256 * 224);
    copy_screen_buffer_grayscale(screen_buffer, &state->memory[0x2400]);
    printf("Loaded screen buffer\n");

    // The raster resolution is 256x224 at 60Hz
    // The monitor is rotated in the cabinet 90 degrees counter-clockwise
    Image image = {
        // .format = (int)PIXELFORMAT_UNCOMPRESSED_GRAYSCALE,
        .format = (int)PIXELFORMAT_UNCOMPRESSED_GRAYSCALE, .mipmaps = 1, .width = 256, .height = 224, .data = screen_buffer,
    };
    /* ImageRotateCCW(&image); */
    /* ImageResize(&image, 224 * scale, 256 * scale); */
    printf("Loaded image\n");

    Texture2D texture;

    double time = GetTime();
    double time_since_last_interrupt = time;
    double interrupt_delay = 1.0 / 120.0;           // delay between two interrupts
    double interrupt_time = time + interrupt_delay; // time of the last interrupt
    state->which_interrupt = 1;

    double dt;
    double last_cycle_time = time;
    int elapsed_cycles;
    int cycle_count;

    /* while (!WindowShouldClose() && iteration_number < 60000) { */
    while (!WindowShouldClose()) {

        // Make sure everything is measured at once
        time = GetTime();
        dt = time - last_cycle_time;
        elapsed_cycles = floor(dt * (double)CLOCK_SPEED);
        time_since_last_interrupt = time - interrupt_time;

        cycle_count = 0;
        while (cycle_count < elapsed_cycles) {

            opcode = &state->memory[state->pc];
            cycle_count += cycles_lookup[opcode[0]];

            if (*opcode == IN) {
                uint8_t port = opcode[1];
                EmulateMachineIn(machine, port);
            } else if (*opcode == OUT) {
                uint8_t port = opcode[1];
                EmulateMachineOut(machine, port);
            } else {
                Emulate8080Op(state);
            }
            i8080_step(c);

            if (!compare_states(c, state)) {
                printf("States are different.\n");
                exit(0);
            }

            /* printf("iteration: %ld\n", iteration_number); */
            iteration_number++;
        }

        // Generate 2 screen interrupts per frame (60Hz so every 1/120s)
        if (time_since_last_interrupt > interrupt_delay) {
            printf("Generating Interrupt\n");

            // If I understand this right then the system gets RST 8 when the beam is *near* the middle of the screen
            // and RST 10 when it is at the end (start of VBLANK).
            GenerateInterrupt(state, state->which_interrupt);

            i8080_interrupt(c, (state->which_interrupt == 2) ? 0xd7 : 0xcf);
            i8080_step(c);

            if (!compare_states(c, state)) {
                printf("States after interrupt are different.\n");
            }

            // Draw on RST 10 interrupt
            if (state->which_interrupt == 1) {
                printf("Drawing screen at iteration %ld\n", iteration_number);

                copy_screen_buffer_grayscale(screen_buffer, &state8080_memory[0x2400]);
                image.data = screen_buffer;

                texture = LoadTextureFromImage(image);

                BeginDrawing();
                ClearBackground(RED);
                DrawTexture(texture, (width - 210 * scale) / 2, (height - 256 * scale) / 2, WHITE);
                EndDrawing();

                UnloadTexture(texture);

                interrupt_time = time; // use the same time as the other measures
            }

            // alternate between 1 and 2 */
            state->which_interrupt = (state->which_interrupt == 1) ? 2 : 1;
        }

        /* if (!compare_states(c, state)) { */
        /*     printf("n = %ld\n", iteration_number); */
        /*     printf("States are different\n"); */
        /*     printf("--------- MY STATE -----------\n"); */
        /*     print_state(state); */
        /*     printf("------BENCHMARK STATE -------\n"); */
        /*     i8080_debug_output(c, 1); */
        /*     exit(1); */
        /* } */

        /* Disassemble8080Op(state->memory, state->pc); */
        /* print_state(state); */
        /* i8080_debug_output(c, 1); */
    }

    UnloadTexture(texture);

    printf("Emulation done.\n");
    compare_memories(i8080_memory, state8080_memory);

    CloseWindow();
    return 0;
}
