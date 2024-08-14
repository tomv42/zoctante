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
    /* case 0: */
    /*     break; */
    /* case 1: */
    /*     state->a = machine->ports.port1; */
    /*     break; */
    /* case 2: */
    /*     state->a = machine->ports.port2; */
    /*     break; */
    /* case 3: { */
    /*     uint16_t v = (machine->hardware_shift.shift_high << 8) | machine->hardware_shift.shift_low; */
    /*     state->a = ((v >> (8 - machine->hardware_shift.shift_offset)) & 0xff); */
    /* } break; */
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
                /* printf("%c", rb(state, addr++)); */
                printf("%c", state->memory[addr++]);

                /* } while (rb(state, addr) != '$'); */
            } while (state->memory[addr] != '$');
        }
    } break;
    /* case 2: */
    /*     machine->hardware_shift.shift_offset = state->a & 7; */
    /*     break; */
    /* case 4: */
    /*     machine->hardware_shift.shift_low = machine->hardware_shift.shift_high; */
    /*     machine->hardware_shift.shift_high = state->a; */
    /*     break; */
    default:
        printf("TestMachineOut %d not implemented.\n", port);
        exit(1);
    }
    return;
}
