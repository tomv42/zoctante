#include "./raylib/include/raylib.h"
#include "8080.h"
#include "comparison.h"
#include "i8080.h"
#include <stdint.h>
/* #include <stdint> */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CPUDIAG 0
#define TEST 0

void EmulateMachineIn(SpaceInvadersMachine *machine, uint8_t port) {
    State8080 *state = machine->state;
    state->pc += 2; // opcode + port
    switch (port) {
    case 0:
        state->a = 1; // play in attract mode TODO: find where this value comes from
        break;
    case 1:
        state->a = 0; // play in attract mode
        break;
    case 3: {
        uint16_t v = (machine->shift_high << 8) | machine->shift_low;
        state->a = ((v >> (8 - machine->shift_offset)) & 0xff);
    } break;
    }
    return;
}

void EmulateMachineOut(SpaceInvadersMachine *machine, uint8_t port) {
    State8080 *state = machine->state;
    state->pc += 2; // opcode + port
    switch (port) {
    case 0:
        state->test_finished = 1;
        break;
    case 1: {
        uint8_t operation = state->c;

        if (operation == 2) { // print a character stored in E
            printf("%c", state->e);
        } else if (operation == 9) { // print from memory at (DE) until '$' char
            uint16_t addr = (state->d << 8) | state->e;
            do {
                /* printf("%c", rb(state, addr++)); */
                printf("%c", state->memory[addr++]);

                /* } while (rb(state, addr) != '$'); */
            } while (state->memory[addr] != '$');
        }
    } break;
    case 2:
        machine->shift_offset = state->a;
        break;
    case 4:
        machine->shift_low = machine->shift_high;
        machine->shift_high = state->a;
        break;
    }
    return;
}

void GenerateInterrupt(State8080 *state, int interrupt_number) {
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

void copy_screen_buffer_grayscale(uint8_t *screen_buffer, uint8_t *memory) {
    for (int i = 0; i < 256 * 224 / 8; i++) {
        uint8_t byte = memory[i];
        for (int j = 0; j < 8; j++) {
            uint8_t bit = (byte >> j) % 2;
            screen_buffer[8 * i + j] = 0xff * bit;
        }
    }
}

int main() {
#if CPUDIAG
    State8080 *state = init_state_8080();
    read_rom_into_memory(state, "cpudiag.bin", 0x100);

    // TODO: Understand these fixes
    state->pc = 0x100;

    // fix the first instruction to be JMP 0x100
    /* state->memory[0] = 0xc3; */
    /* state->memory[1] = 0; */
    /* state->memory[2] = 0x01; */

    // fix the stack pointer from 0x6ad to 0x7ad
    // this 0x06 byte 112 in the code, which is
    // byte 112 + 0x100 = 368 in memory
    state->memory[368] = 0x7;

    // skip DAA test
    state->memory[0x59c] = 0xc3; // JMP
    state->memory[0x59d] = 0xc2;
    state->memory[0x59e] = 0x05;

    // inject "out 0,a" at 0x0000 (signal to stop the test)
    state->memory[0x0000] = 0xD3;
    state->memory[0x0001] = 0x00;

    // inject "out 1,a" at 0x0005 (signal to output some characters)
    state->memory[0x0005] = 0xD3;
    state->memory[0x0006] = 0x01;
    state->memory[0x0007] = 0xC9;

    SpaceInvadersMachine *machine = init_machine();
    machine->state = state;

    long n = 0;
    uint8_t *opcode;

    while (!state->test_finished) {
        if (n % 1000000 == 0) {
            printf("n=%ld\n", n);
        }
        opcode = &state->memory[state->pc];
        if (*opcode == IN) {
            uint8_t port = opcode[1];
            EmulateMachineIn(machine, port);
        } else if (*opcode == OUT) {
            uint8_t port = opcode[1];
            EmulateMachineOut(machine, port);
        } else {
            /* Disassemble8080Op(state->memory, state->pc); */
            /* print_state(state); */
            Emulate8080Op(state);
        }
        n++;
    }

    printf("\n");
    Disassemble8080Op(state->memory, 0x01C2);

    return 0;
#elif TEST
    State8080 *state = init_state_8080();
    read_rom_into_memory(state, "8080PRE.COM", 0x0100);

    printf("Test: 8080PRE.COM\n");

    state->pc = 0x100;

    // inject "out 0,a" at 0x0000 (signal to stop the test)
    state->memory[0x0000] = 0xD3;
    state->memory[0x0001] = 0x00;

    // inject "out 1,a" at 0x0005 (signal to output some characters)
    state->memory[0x0005] = 0xD3;
    state->memory[0x0006] = 0x01;
    state->memory[0x0007] = 0xC9;

    SpaceInvadersMachine *machine = init_machine();
    machine->state = state;

    long n = 0;
    uint8_t *opcode;

    while (!state->test_finished) {
        if (n % 1000000 == 0) {
            printf("n=%ld\n", n);
        }
        opcode = &state->memory[state->pc];
        if (*opcode == IN) {
            uint8_t port = opcode[1];
            EmulateMachineIn(machine, port);
        } else if (*opcode == OUT) {
            uint8_t port = opcode[1];
            EmulateMachineOut(machine, port);
        } else {
            /* Disassemble8080Op(state->memory, state->pc); */
            /* print_state(state); */
            Emulate8080Op(state);
        }
        n++;
    }

    Disassemble8080Op(state->memory, 0x0255);

    printf("\n");

    return 0;

#else
    // TODO: move this inside the init_machine function?
    State8080 *state = init_state_8080();
    /* read_rom_into_memory(state, "space-invaders.rom", 0x0000); */
    read_rom_into_memory(state, "invaders.concatenated", 0x0000);

    SpaceInvadersMachine *machine = init_machine();
    machine->state = state;

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
    // no need to call UnloadImage

    Texture2D texture;
    /* Texture2D texture = LoadTextureFromImage(image); */
    /* printf("Loaded Texture\n"); */

    double time_since_last_interrupt = 0.0;
    double interrupt_time = 2.0;
    state->which_interrupt = 1;

    /* while (iteration_number < 60000) { */
    /* while (!WindowShouldClose()) */
    while (!WindowShouldClose() && iteration_number < 60000) {
        // Generate screen interrupts
        time_since_last_interrupt = GetTime() - interrupt_time;
        if (time_since_last_interrupt > 1.0 / 60.0) {
            /* printf("Generating Interrupt\n"); */
            /* if (!compare_states(c, state)) { */
            /*     printf("States before interrupt are not the same.\n"); */
            /* } else { */
            /*     printf("States before interrupt are the same.\n"); */
            /* } */

            // If I understand this right then the system gets RST 8 when the beam is *near* the middle of the screen
            // and RST 10 when it is at the end (start of VBLANK).
            GenerateInterrupt(state, state->which_interrupt);

            i8080_interrupt(c, (state->which_interrupt == 2) ? 0xd7 : 0xcf);
            i8080_step(c);

            /* if (!compare_states(c, state)) { */
            /*     printf("States after interrupt are not the same.\n"); */
            /* } else { */
            /*     printf("States after interrupt are the same.\n"); */
            /* } */

            state->which_interrupt = 3 - state->which_interrupt; // alternate between 1 and 2

            // Draw on RST 10 interrupt
            if (state->which_interrupt == 10) {
                copy_screen_buffer_grayscale(screen_buffer, &state8080_memory[0x2400]);
                /* image.format = (int)PIXELFORMAT_UNCOMPRESSED_GRAYSCALE; */
                /* image.height = 224; */
                /* image.width = 256; */
                /* image.mipmaps = 1; */
                image.data = screen_buffer;

                texture = LoadTextureFromImage(image);

                BeginDrawing();
                ClearBackground(RED);
                DrawTexture(texture, (width - 210 * scale) / 2, (height - 256 * scale) / 2, WHITE);
                EndDrawing();

                UnloadTexture(texture);
                interrupt_time = GetTime();
            }
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

        opcode = &state->memory[state->pc];
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

        /* Disassemble8080Op(state->memory, state->pc); */
        /* print_state(state); */
        /* i8080_debug_output(c, 1); */

        iteration_number++;
    }

    printf("iteration %ld\n", iteration_number);

    copy_screen_buffer_grayscale(screen_buffer, &state->memory[0x2400]);

    LoadTextureFromImage(image);

    image.data = screen_buffer;

    ImageRotateCCW(&image);
    ImageResize(&image, 224 * scale, 256 * scale);
    texture = LoadTextureFromImage(image);

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        /* copy_screen_buffer_grayscale(screen_buffer, &state8080_memory[0x2400]); */
        /* image.data = screen_buffer; */
        /* texture = LoadTextureFromImage(image); */

        BeginDrawing();
        ClearBackground(RED);
        DrawTexture(texture, (width - 210 * scale) / 2, (height - 256 * scale) / 2, WHITE);
        DrawFPS(10, 10);
        EndDrawing();

        /* UnloadTexture(texture); */
    }

    UnloadTexture(texture);

    printf("Emulation done.\n");
    compare_memories(i8080_memory, state8080_memory);

    CloseWindow();
    return 0;
#endif
}
