#ifndef H_TEST_MACHINE
#define H_TEST_MACHINE

#include <stdint.h>

// typedef struct Ports {
//     uint8_t port0;
//     uint8_t port1;
//     uint8_t port2;
//     uint8_t port3;
//     uint8_t port5;
// } Ports;
//
// typedef struct HardwareShift {
//     uint8_t shift_high;
//     uint8_t shift_low;
//     uint8_t shift_offset;
// } HardwareShift;

typedef struct TestMachine {
    struct State8080 *state;
    // HardwareShift hardware_shift;
    // Ports ports;
    uint8_t test_finished;
} TestMachine;

TestMachine *init_test_machine(void);
void free_test_machine(TestMachine *machine);
void EmulateTestMachineIn(TestMachine *machine, uint8_t port);
void EmulateTestMachineOut(TestMachine *machine, uint8_t port);

#endif
