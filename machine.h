#ifndef H_MACHINE
#define H_MACHINE

#include "raylib/include/raylib.h"
#include <stdint.h>

typedef struct Ports {
    uint8_t port0;
    uint8_t port1;
    uint8_t port2;
    uint8_t port3;
    uint8_t port5;
} Ports;

typedef struct HardwareShift {
    uint8_t shift_high;
    uint8_t shift_low;
    uint8_t shift_offset;
} HardwareShift;

typedef struct SpaceInvadersMachine {
    struct State8080 *state;
    HardwareShift hardware_shift;
    Ports ports;
    Sound sounds[9];
} SpaceInvadersMachine;

SpaceInvadersMachine *init_machine(void);
void free_machine(SpaceInvadersMachine *machine);
void EmulateMachineIn(SpaceInvadersMachine *machine, uint8_t port);
void EmulateMachineOut(SpaceInvadersMachine *machine, uint8_t port);
void emulate_machines_input(SpaceInvadersMachine *machine);

#endif
