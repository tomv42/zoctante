#include "comparison.h"
#include "i8080.h"
#include "machine.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

State8080 *benchmark_state;
uint8_t *memory;
bool test_finished = 0;

static void init_benchmark_memory(void) {
    memory = calloc(1 << 16, 1);
    if (memory == NULL) {
        printf("Couldn't allocate benchmark memory.\n");
        exit(1);
    }
}

static void free_benchmark_memory(void) {
    free(memory);
}

static uint8_t rb(void *userdata, uint16_t addr) {
    return memory[addr];
}

static void wb(void *userdata, uint16_t addr, uint8_t val) {
    memory[addr] = val;
}

static uint8_t port_in(void *userdata, uint8_t port) {
    i8080 *c = (i8080 *)userdata;
    uint8_t value = 0xff;

    switch (port) {
    case 1:
        value = c->ports.port1;
        break;
    case 2:
        value = c->ports.port2;
        break;
    case 3: {
        uint16_t v = (c->shift_high << 8) | c->shift_low;
        value = ((v >> (8 - c->shift_offset)) & 0xff);
    } break;
    default:
        printf("Port not implemented\n");
        exit(1);
    }

    return value;
}

static void port_out(void *userdata, uint8_t port, uint8_t value) {
    i8080 *const c = (i8080 *)userdata;

    switch (port) {
    /* case 0: */
    /*     test_finished = 1; */
    /*     break; */
    case 1: {
        uint8_t operation = c->c;

        if (operation == 2) { // print a character stored in E
            printf("%c", c->e);
        } else if (operation == 9) { // print from memory at (DE) until '$' char
            uint16_t addr = (c->d << 8) | c->e;
            do {
                printf("%c", rb(c, addr++));
            } while (rb(c, addr) != '$');
        }
    } break;
    case 2:
        c->shift_offset = value & 7;
        break;
    case 4:
        c->shift_low = c->shift_high;
        c->shift_high = value;
        break;
    }
}

// ports passed as value
void update_benchmark_ports(i8080 *c, Ports ports) {
    c->ports.port1 = ports.port1;
    c->ports.port2 = ports.port2;
}

// returns 1 if the states are the same, 0 otherwise
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
    } else if (c->hf != state->cc.ac) {
        return 0;
    }

    return 1;
}

int memories_are_equal(uint8_t *i8080_memory, uint8_t *state8080_memory) {
    // 2000-23FF 1K RAM
    // 2400-3FFF 7K Video RAM
    return memcmp(i8080_memory, state8080_memory, 0x3fff - 0x2000) == 0;
};

i8080 *init_benchmark_emulator(char *filename, uint16_t offset) {
    // HACK: The right way to do it would be to initialize the memory immediately
    // without using my emulator to do it in an indirect way
    init_benchmark_memory();
    read_rom_into_memory(memory, filename, offset);

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

void free_benchmark_emulator(i8080 *c) {
    free_benchmark_memory();
    free(c);
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
