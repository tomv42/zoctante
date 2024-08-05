#include "8080.h"
#include "machine.h"
#include <stdio.h>
#include <string.h>

int main(void) {
    char *filename = "cpudiag.bin";
    SpaceInvadersMachine *machine = init_machine();

    State8080 *state = machine->state;
    read_rom_into_memory(state->memory, "cpudiag.bin", 0x100);

    // TODO: Understand these fixes
    state->pc = 0x100;

    // fix the first instruction to be JMP 0x100
    /* state->memory[0] = 0xc3; */
    /* state->memory[1] = 0; */
    /* state->memory[2] = 0x01; */

    if (strcmp(filename, "cpudiag.bin")) {
        // fix the stack pointer from 0x6ad to 0x7ad
        // this 0x06 byte 112 in the code, which is
        // byte 112 + 0x100 = 368 in memory
        state->memory[368] = 0x7;
        // skip DAA test
        state->memory[0x59c] = 0xc3; // JMP
        state->memory[0x59d] = 0xc2;
        state->memory[0x59e] = 0x05;
    }

    // inject "out 0,a" at 0x0000 (signal to stop the test)
    state->memory[0x0000] = 0xD3;
    state->memory[0x0001] = 0x00;

    // inject "out 1,a" at 0x0005 (signal to output some characters)
    state->memory[0x0005] = 0xD3;
    state->memory[0x0006] = 0x01;
    state->memory[0x0007] = 0xC9;

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
}
