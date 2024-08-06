#include "machine.h"
#include "8080.h"
#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

SpaceInvadersMachine *init_machine(void) {
    SpaceInvadersMachine *machine = malloc(sizeof(*machine));
    memset(machine, 0, sizeof(*machine));

    machine->state = init_state_8080();

    machine->sounds[0] = LoadSound("./data/ufo_highpitch.wav"); // UFO
    machine->sounds[1] = LoadSound("./data/shoot.wav");         // Shot
    machine->sounds[2] = LoadSound("./data/explosion.wav");     // Player has been hit
    machine->sounds[3] = LoadSound("./data/invaderkilled.wav"); // Invader has been hit
    machine->sounds[4] = LoadSound("./data/fastinvader1.wav");  // Fleet 1
    machine->sounds[5] = LoadSound("./data/fastinvader2.wav");  // Fleet 2
    machine->sounds[6] = LoadSound("./data/fastinvader3.wav");  // Fleet 3
    machine->sounds[7] = LoadSound("./data/fastinvader4.wav");  // Fleet 4
    machine->sounds[8] = LoadSound("./data/ufo_lowpitch.wav");  // UFO hit

    return machine;
}

void free_machine(SpaceInvadersMachine *machine) {
    free_state_8080(machine->state);
    UnloadSound(machine->sounds[0]);
    UnloadSound(machine->sounds[1]);
    UnloadSound(machine->sounds[2]);
    UnloadSound(machine->sounds[3]);
    UnloadSound(machine->sounds[4]);
    UnloadSound(machine->sounds[5]);
    UnloadSound(machine->sounds[6]);
    UnloadSound(machine->sounds[7]);
    UnloadSound(machine->sounds[8]);
    free(machine);
}

static void play_sound(SpaceInvadersMachine *machine, int sound_index) {
    if (!IsSoundPlaying(machine->sounds[sound_index])) {
        PlaySound(machine->sounds[sound_index]);
    }
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
    case 3: {
        uint8_t new_port3 = state->a;
        uint8_t last_port3 = machine->ports.port3;

        // play a sound when a bit goes from 0 to 1
        if (new_port3 != last_port3) {
            if ((new_port3 & 0x02) && !(last_port3 & 0x02)) {
                play_sound(machine, 1);
            } else if ((new_port3 & 0x04) && !(last_port3 & 0x04)) {
                play_sound(machine, 2);
            } else if ((new_port3 & 0x08) && !(last_port3 & 0x08)) {
                play_sound(machine, 3);
            }
        }

        // except for the ufo sound that repeats
        if (new_port3 & 0x01) {
            play_sound(machine, 0);
        }

        machine->ports.port3 = new_port3;
    } break;
    case 4:
        machine->hardware_shift.shift_low = machine->hardware_shift.shift_high;
        machine->hardware_shift.shift_high = state->a;
        break;
    case 5: {
        uint8_t new_port5 = state->a;
        uint8_t last_port5 = machine->ports.port5;

        // play a sound when port5 goes from 0 to 1
        if (new_port5 != last_port5) {
            if ((new_port5 & 0x01) && !(last_port5 & 0x01)) {
                play_sound(machine, 4);
            } else if ((new_port5 & 0x02) && !(last_port5 & 0x02)) {
                play_sound(machine, 5);
            } else if ((new_port5 & 0x04) && !(last_port5 & 0x04)) {
                play_sound(machine, 6);
            } else if ((new_port5 & 0x08) && !(last_port5 & 0x08)) {
                play_sound(machine, 7);
            } else if ((new_port5 & 0x10) && !(last_port5 & 0x10)) {
                play_sound(machine, 8);
            }
        }

        machine->ports.port5 = new_port5;
    } break;
    }
    return;
}

static void update_ports_from_input(Ports *ports) {
    ports->port1 |= 1 << 3; // always 1
    if (IsKeyDown(KEY_C)) {
        ports->port1 |= 1 << 0; // CREDIT
    } else if (IsKeyDown(KEY_TWO)) {
        ports->port1 |= 1 << 1; // 2P start
    } else if (IsKeyDown(KEY_ONE)) {
        ports->port1 |= 1 << 2; // 1P start
    } else if (IsKeyDown(KEY_SPACE)) {
        ports->port1 |= 1 << 4; // 1P shoot
        ports->port2 |= 1 << 4; // 2P shoot
    } else if (IsKeyDown(KEY_LEFT)) {
        ports->port1 |= 1 << 5; // 1P left
        ports->port2 |= 1 << 5; // 2P left
    } else if (IsKeyDown(KEY_RIGHT)) {
        ports->port1 |= 1 << 6; // 1P right
        ports->port2 |= 1 << 6; // 2P right
    } else if (IsKeyDown(KEY_T)) {
        ports->port2 |= 1 << 2; // Tilt
    }
    if (IsKeyUp(KEY_C)) {
        ports->port1 &= ~(1 << 0); // CREDIT
    }
    if (IsKeyUp(KEY_TWO)) {
        ports->port1 &= ~(1 << 1); // 2P start
    }
    if (IsKeyUp(KEY_ONE)) {
        ports->port1 &= ~(1 << 2); // 1P start
    }
    if (IsKeyUp(KEY_SPACE)) {
        ports->port1 &= ~(1 << 4); // 1P shoot
        ports->port2 &= ~(1 << 4); // 2P shoot
    }
    if (IsKeyUp(KEY_LEFT)) {
        ports->port1 &= ~(1 << 5); // 1P left
        ports->port2 &= ~(1 << 5); // 2P left
    }
    if (IsKeyUp(KEY_RIGHT)) {
        ports->port1 &= ~(1 << 6); // 1P right
        ports->port2 &= ~(1 << 6); // 2P right
    }
    if (IsKeyUp(KEY_T)) {
        ports->port2 &= ~(1 << 2); // Tilt
    }
}

void emulate_machines_input(SpaceInvadersMachine *machine) {
    // take care of the inputs that affects ports
    update_ports_from_input(&machine->ports);
    // other inputs
}
