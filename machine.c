#include "machine.h"
#include "./raylib/include/raylib.h"
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

void free_machine(SpaceInvadersMachine *machine) {
    free_state_8080(machine->state);
    free(machine);
}

void EmulateMachineIn(SpaceInvadersMachine *machine, uint8_t port) {
    State8080 *state = machine->state;
    state->pc += 2; // opcode + port
    switch (port) {
    case 0:
        break;
    case 1:
        state->a = machine->ports.port1;
        break;
    case 2:
        state->a = machine->ports.port2;
        break;
    case 3: {
        uint16_t v = (machine->hardware_shift.shift_high << 8) | machine->hardware_shift.shift_low;
        state->a = ((v >> (8 - machine->hardware_shift.shift_offset)) & 0xff);
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
        machine->hardware_shift.shift_offset = state->a & 7;
        break;
    case 4:
        machine->hardware_shift.shift_low = machine->hardware_shift.shift_high;
        machine->hardware_shift.shift_high = state->a;
        break;
    }
    return;
}

void update_ports_from_input(Ports *ports) {
    ports->port1 |= 1 << 3; // always 1
    if (IsKeyDown(KEY_C)) {
        ports->port1 |= 1 << 0;
    } else if (IsKeyDown(KEY_ZERO)) {
        ports->port1 |= 1 << 1;
    } else if (IsKeyDown(KEY_ONE)) {
        ports->port1 |= 1 << 2;
    } else if (IsKeyDown(KEY_SPACE)) {
        ports->port1 |= 1 << 4;
        ports->port2 |= 1 << 4;
    } else if (IsKeyDown(KEY_LEFT)) {
        ports->port1 |= 1 << 5;
        ports->port2 |= 1 << 5;
    } else if (IsKeyDown(KEY_RIGHT)) {
        ports->port1 |= 1 << 6;
        ports->port2 |= 1 << 6;
    } else if (IsKeyDown(KEY_T)) {
        ports->port2 |= 1 << 2;
    }
    if (IsKeyUp(KEY_C)) {
        ports->port1 &= ~(1 << 0);
    }
    if (IsKeyUp(KEY_ZERO)) {
        ports->port1 &= ~(1 << 1);
    }
    if (IsKeyUp(KEY_ONE)) {
        ports->port1 &= ~(1 << 2);
    }
    if (IsKeyUp(KEY_SPACE)) {
        ports->port1 &= ~(1 << 4);
        ports->port2 &= ~(1 << 4);
    }
    if (IsKeyUp(KEY_LEFT)) {
        ports->port1 &= ~(1 << 5);
        ports->port2 &= ~(1 << 5);
    }
    if (IsKeyUp(KEY_RIGHT)) {
        ports->port1 &= ~(1 << 6);
        ports->port2 &= ~(1 << 6);
    }
    if (IsKeyUp(KEY_T)) {
        ports->port2 &= ~(1 << 2);
    }
}

void emulate_machines_input(SpaceInvadersMachine *machine) {
    // take care of the inputs that affects ports
    update_ports_from_input(&machine->ports);
    // other inputs
}
