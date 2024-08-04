#ifndef H_MACHINE
#define H_MACHINE

#include <stdint.h>

typedef struct SpaceInvadersMachine {
    struct State8080 *state;
    uint8_t shift_high;
    uint8_t shift_low;
    uint8_t shift_offset;
} SpaceInvadersMachine;

SpaceInvadersMachine *init_machine(void);
void EmulateMachineIn(SpaceInvadersMachine *machine, uint8_t port);
void EmulateMachineOut(SpaceInvadersMachine *machine, uint8_t port);

#endif
