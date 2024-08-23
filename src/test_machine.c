#include "test_machine.h"
#include "8080.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

TestMachine *init_test_machine(void) {
    TestMachine *machine = malloc(sizeof(*machine));
    memset(machine, 0, sizeof(*machine));

    machine->state = init_state_8080();
    return machine;
}

void free_test_machine(TestMachine *machine) {
    free_state_8080(machine->state);
    free(machine);
}

void EmulateTestMachineIn(TestMachine *machine, uint8_t port) {
    State8080 *state = machine->state;
    state->pc += 2; // opcode + port
    switch (port) {
    default:
        printf("TestMachineIn %d not implemented.\n", port);
        exit(1);
    }
    return;
}

void EmulateTestMachineOut(TestMachine *machine, uint8_t port) {
    State8080 *state = machine->state;
    state->pc += 2; // opcode + port
    switch (port) {
    case 0:
        machine->test_finished = 1;
        break;
    case 1: {
        uint8_t operation = state->c;

        if (operation == 2) { // print a character stored in E
            printf("%c", state->e);
        } else if (operation == 9) { // print from memory at (DE) until '$' char
            uint16_t addr = (state->d << 8) | state->e;
            do {
                printf("%c", state->memory[addr++]);

            } while (state->memory[addr] != '$');
        }
    } break;
    default:
        printf("TestMachineOut %d not implemented.\n", port);
        exit(1);
    }
    return;
}
