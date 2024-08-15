#include "8080.h"
#include "test_machine.h"
#include <stdint.h>
#include <stdio.h>

int main(void) {
    char *filename = "./tests/8080EXM.COM";

    printf("Testing %s\n", filename);
    printf("\n--------------------------\n");

    TestMachine *machine = init_test_machine();

    State8080 *state = machine->state;
    read_rom_into_memory(state->memory, filename, 0x100);

    // start at 0x100
    state->pc = 0x100;

    // inject a machine out on port 0 at 0x0000 as a signal to stop the test
    state->memory[0x0000] = 0xD3;
    state->memory[0x0001] = 0x00;

    // inject a machine out on port 1 at 0x0005 as a signal to output some characters
    state->memory[0x0005] = 0xD3;
    state->memory[0x0006] = 0x01;
    state->memory[0x0007] = 0xC9;

    uint8_t *opcode;

    while (!machine->test_finished) {
        opcode = &state->memory[state->pc];
        if (*opcode == IN) {
            uint8_t port = opcode[1];
            EmulateTestMachineIn(machine, port);
        } else if (*opcode == OUT) {
            uint8_t port = opcode[1];
            EmulateTestMachineOut(machine, port);
        } else {
            Emulate8080Op(state);
        }
    }

    printf("\n--------------------------\n");
    printf("\nTest finished.\n");

    return 0;
}
