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

static void copy_screen_buffer_grayscale(uint8_t *screen_buffer, uint8_t *memory, int scale) {
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

    /* SetTraceLogLevel(LOG_WARNING); */
    InitWindow(width, height, "zoctante - space invaders");
    SetWindowMonitor(0);
    SetWindowPosition(10, 10);

    // 2400-3FFF 7K video RAM
    /* uint8_t *screen_buffer = &state->memory[0x2400]; */
    /* uint8_t *screen_buffer = memory + 0x2400; */
    uint8_t *screen_buffer = malloc(scale * 8 * 256 * 224);
    copy_screen_buffer_grayscale(screen_buffer, &state->memory[0x2400], scale);
    printf("Loaded screen buffer\n");

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

    double time = GetTime();
    double time_since_last_interrupt = time;
    // HACK: I don't know why it's not 1/120 to get 60FPS
    double interrupt_delay = 2.0 / 120.0;           // delay between two interrupts
    double interrupt_time = time + interrupt_delay; // time of the last interrupt
    state->which_interrupt = 1;

    double dt;
    double last_cycle_time = time;
    int elapsed_cycles;
    int cycle_count;
    uint16_t pc;

    while (!WindowShouldClose()) {

        // Make sure everything is measured at once
        time = GetTime();
        dt = time - last_cycle_time;
        elapsed_cycles = floor(dt * (double)CLOCK_SPEED);
        time_since_last_interrupt = time - interrupt_time;

        cycle_count = 0;
        while (cycle_count < elapsed_cycles) {

#if 0
            Disassemble8080Op(state->memory, pc);
            printf("\n--------- MY STATE -----------\n");
            print_state(state);
            printf("\n------ BENCHMARK STATE -------\n");
            i8080_debug_output(c, 1);
#endif

            pc = state->pc; // store pc for debugging purposes
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
            i8080_step(c);

            if (!compare_states(c, state)) {
                printf("\nn = %ld\n", iteration_number);
                printf("\nStates are different after instruction:\n");
                Disassemble8080Op(state->memory, pc);
                print_state_comparison(state, c);
                exit(1);
            }

            int n = compare_memories(i8080_memory, state8080_memory);

            if (n > 0) {
                printf("\nMemories differ in %d place(s) after instruction:.\n", n);
                Disassemble8080Op(state->memory, pc);
                print_state_comparison(state, c);
                exit(1);
            }

            /* printf("iteration: %ld\n", iteration_number); */
            iteration_number++;
        }

        last_cycle_time = GetTime();

        // Generate 2 screen interrupts per frame (60Hz so every 1/120s)
        if (time_since_last_interrupt > interrupt_delay) {
            /* printf("\nGenerating Interrupt\n"); */

            // If I understand this right then the system gets RST 8 when the beam is *near* the middle of the screen
            // and RST 10 when it is at the end (start of VBLANK).

            GenerateInterrupt(state, state->which_interrupt);
            i8080_interrupt(c, (state->which_interrupt == 2) ? 0xd7 : 0xcf);

            /* if (!compare_states(c, state)) { */
            /*     printf("\nStates after interrupt are different.\n"); */
            /*     print_state_comparison(state, c); */
            /*     printf("\nnext instruction:\n"); */
            /*     Disassemble8080Op(state->memory, state->pc); */
            /*     exit(1); */
            /* } */

            // Draw on RST 10 interrupt
            if (state->which_interrupt == 1) {
                /* printf("Drawing screen at iteration %ld\n", iteration_number); */

                copy_screen_buffer_grayscale(screen_buffer, &state8080_memory[0x2400], scale);
                /* copy_screen_buffer_grayscale(screen_buffer, &i8080_memory[0x2400], scale); */

                UpdateTexture(texture, image.data);

                BeginDrawing();
                ClearBackground(BLACK);
                DrawTexture(texture, (width - 224 * scale) / 2, (height - 256 * scale) / 2, WHITE);
                DrawFPS(50, 10);
                EndDrawing();

                interrupt_time = time; // use the same time as the other measures
            }

            // alternate between 1 and 2 */
            state->which_interrupt = (state->which_interrupt == 1) ? 2 : 1;
        }
    }

    UnloadTexture(texture);

    printf("Emulation done.\n");
    compare_memories(i8080_memory, state8080_memory);

    CloseWindow();
    return 0;
}
