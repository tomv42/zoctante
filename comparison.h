#ifndef H_COMPARISON
#define H_COMPARISON

#include "8080.h"
#include "i8080.h"

// TODO: make it backend agnostic?

bool compare_states(i8080 *c, State8080 *state);
int memories_are_equal(uint8_t *i8080_memory, uint8_t *state8080_memory);
i8080 *init_benchmark_emulator(char *filename, uint16_t offset);
uint8_t *get_benchmark_memory(void);
void print_state_comparison(State8080 *state, i8080 *c);

#endif
