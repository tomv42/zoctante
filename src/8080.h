#ifndef H_8080
#define H_8080

#include <stdbool.h>
#include <stdint.h>

#define IN (0xdb)  // opcode of IN
#define OUT (0xd3) // opcode of OUT

typedef struct ConditionCodes {
    uint8_t z : 1;
    uint8_t s : 1;
    uint8_t p : 1;
    uint8_t cy : 1;
    uint8_t ac : 1;
    uint8_t pad : 3;
} ConditionCodes;

typedef struct State8080 {
    uint8_t a;
    uint8_t b;
    uint8_t c;
    uint8_t d;
    uint8_t e;
    uint8_t h;
    uint8_t l;
    uint16_t sp;
    uint16_t pc;
    uint8_t *memory;
    struct ConditionCodes cc;
    uint8_t int_enable;
    uint8_t int_pending;
    uint8_t int_number;
    uint8_t int_delay;
    uint8_t which_interrupt;
    uint32_t iteration_number;
} State8080;

State8080 *init_state_8080(void);
void free_state_8080(State8080 *state);
void print_state(State8080 *state);
int Disassemble8080Op(unsigned char *codebuffer, int pc);
void Emulate8080Op(State8080 *state);
void read_rom_into_memory(uint8_t *memory, char *filename, uint16_t offset);
uint8_t AuxiliaryCarry(uint16_t answer, uint8_t a, uint8_t b);

#endif
