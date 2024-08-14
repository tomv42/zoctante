#include "8080.h"
#include "comparison.h"
#include "i8080.h"
#include "test_machine.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    char *filename = "./tests/8080EXM.COM";
    TestMachine *machine = init_test_machine();

    State8080 *state = machine->state;
    read_rom_into_memory(state->memory, filename, 0x100);

    i8080 *c = init_benchmark_emulator(filename, 0x100);
    uint8_t *i8080_memory = get_benchmark_memory();

    // TODO: Understand these fixes

    // fix the first instruction to be JMP 0x100
    state->pc = 0x100;
    c->pc = 0x100;
    /* state->memory[0] = 0xc3; */
    /* state->memory[1] = 0; */
    /* state->memory[2] = 0x01; */

    if (strcmp(filename, "./tests/cpudiag.bin")) {
        // fix the stack pointer from 0x6ad to 0x7ad
        // this 0x06 byte 112 in the code, which is
        // byte 112 + 0x100 = 368 in memory
        /* state->memory[368] = 0x7; */
        /* i8080_memory[368] = 0x7; */
    }

    // inject "out 0,a" at 0x0000 (signal to stop the test)
    state->memory[0x0000] = 0xD3;
    state->memory[0x0001] = 0x00;
    i8080_memory[0x0000] = 0xD3;
    i8080_memory[0x0001] = 0x00;

    // inject "out 1,a" at 0x0005 (signal to output some characters)
    state->memory[0x0005] = 0xD3;
    state->memory[0x0006] = 0x01;
    state->memory[0x0007] = 0xC9;
    i8080_memory[0x0005] = 0xD3;
    i8080_memory[0x0006] = 0x01;
    i8080_memory[0x0007] = 0xC9;

    long n = 0;
    uint8_t *opcode;
    uint16_t pc;

    while (!machine->test_finished) {
        /* if (n % 1000000 == 0) { */
        /*     printf("n=%ld\n", n); */
        /* } */
        opcode = &state->memory[state->pc];
        pc = state->pc;
        /* print_state(state); */
        /* Disassemble8080Op(state->memory, state->pc); */
        if (*opcode == IN) {
            uint8_t port = opcode[1];
            EmulateTestMachineIn(machine, port);
        } else if (*opcode == OUT) {
            uint8_t port = opcode[1];
            EmulateTestMachineOut(machine, port);
        } else {
            Emulate8080Op(state);
        }
        i8080_step(c);

        if (!compare_states(c, state)) {
            printf("\nn = %ld\n", n);
            printf("\nStates are different after instruction:\n");
            Disassemble8080Op(state->memory, pc);
            print_state_comparison(state, c);
            exit(1);
        }
        /* if (!memories_are_equal(i8080_memory, state->memory)) { */
        /*     printf("\nMemories differ after instruction:\n"); */
        /*     Disassemble8080Op(state->memory, pc); */
        /*     print_state_comparison(state, c); */
        /*     exit(1); */
        /* } */
        n++;
    }

    printf("\n");

    /* pc = 0x0251; */
    /* for (int i = 0; i < 10; i++) { */
    /*     pc += Disassemble8080Op(state->memory, pc); */
    /* } */
    /**/
    /* free_test_machine(machine); */
    /* free_benchmark_emulator(c); */

    printf("Test finished.\n");

    return 0;
}
