#include "comparison.h"
#include "i8080.h"
#include <stdint.h>
#include <stdlib.h>

State8080 *benchmark_state;
uint8_t *memory;
bool test_finished = 0;

static uint8_t rb(void *userdata, uint16_t addr) {
    return memory[addr];
}

static void wb(void *userdata, uint16_t addr, uint8_t val) {
    memory[addr] = val;
}

static uint8_t port_in(void *userdata, uint8_t port) {
    return 0x00;
}

static void port_out(void *userdata, uint8_t port, uint8_t value) {
    i8080 *const c = (i8080 *)userdata;

    if (port == 0) {
        test_finished = 1;
    } else if (port == 1) {
        uint8_t operation = c->c;

        if (operation == 2) { // print a character stored in E
            printf("%c", c->e);
        } else if (operation == 9) { // print from memory at (DE) until '$' char
            uint16_t addr = (c->d << 8) | c->e;
            do {
                printf("%c", rb(c, addr++));
            } while (rb(c, addr) != '$');
        }
    }
}

bool compare_states(i8080 *c, State8080 *state) {
    // registers
    if (c->pc != state->pc) {
        printf("PC=%04x   =!=    PC=%04x\n", c->pc, state->pc);
        return 0;
    } else if (c->a != state->a) {
        return 0;
    } else if (c->b != state->b) {
        return 0;
    } else if (c->c != state->c) {
        return 0;
    } else if (c->d != state->d) {
        return 0;
    } else if (c->e != state->e) {
        return 0;
    } else if (c->h != state->h) {
        return 0;
    } else if (c->l != state->l) {
        return 0;
    } else if (c->pc != state->pc) {
        return 0;
    } else if (c->sp != state->sp) {
        return 0;
    } else if (c->sf != state->cc.s) {
        return 0;
    } else if (c->cf != state->cc.cy) {
        return 0;
    } else if (c->pf != state->cc.p) {
        return 0;
    } else if (c->zf != state->cc.z) {
        return 0;
    }

    return 1;
}

// returns the number of places where memory differs
int compare_memories(uint8_t *i8080_memory, uint8_t *state8080_memory) {
    int nb_diffs = 0;
    for (int i = 0; i < 1 << 16; i++) {
        nb_diffs += (i8080_memory[i] != state8080_memory[i]);
    }
    /* printf("Memory differs in %d places.\n", nb_diffs); */
    return nb_diffs;
};

i8080 *init_benchmark_emulator(char *filename, uint16_t offset) {
    // HACK: The right way to do it would be to initialize the memory immediately
    // without using my emulator to do it in an indirect way
    benchmark_state = init_state_8080();
    read_rom_into_memory(benchmark_state, filename, offset);
    memory = benchmark_state->memory;

    i8080 *c = malloc(sizeof(*c));
    i8080_init(c); // need to supply the read and write functions

    c->userdata = c;
    c->read_byte = rb;
    c->write_byte = wb;
    c->port_in = port_in;
    c->port_out = port_out;

    c->sp = 0xf000;

    return c;
}

uint8_t *get_benchmark_memory(void) {
    return memory;
}

void print_state_comparison(State8080 *state, i8080 *c) {
    printf("\n--------- MY STATE -----------\n");
    print_state(state);
    printf("\n------ BENCHMARK STATE -------\n");
    i8080_debug_output(c, 1);
}
