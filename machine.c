#include "machine.h"
#include "8080.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

SpaceInvadersMachine *init_machine(void) {
    SpaceInvadersMachine *machine = malloc(sizeof(*machine));
    memset(machine, 0, sizeof(*machine));
    machine->state = init_state_8080();
    return machine;
}

void EmulateMachineIn(SpaceInvadersMachine *machine, uint8_t port) {
    State8080 *state = machine->state;
    state->pc += 2; // opcode + port
    switch (port) {
    case 0:
        state->a = 1; // play in attract mode
                      // TODO: find where this value comes from
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
