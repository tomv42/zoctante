#include "8080.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

State8080 *init_state_8080(void) {
    State8080 *state = malloc(sizeof(*state));
    memset(state, 0, sizeof(*state));
    uint8_t *memory = calloc(1 << 16, 1);
    if (memory == NULL) {
        printf("Couldn't allocate memory for 8080 state.\n");
        exit(1);
    }
    state->memory = memory;

    state->sp = 0xf000;

    return state;
}

void free_state_8080(State8080 *state) {
    free(state->memory);
    free(state);
}

void print_state(State8080 *state) {

    uint8_t f = 0;
    f |= state->cc.s << 7;
    f |= state->cc.z << 6;
    f |= state->cc.ac << 4;
    f |= state->cc.p << 2;
    f |= 1 << 1; // bit 1 is always 1
    f |= state->cc.cy << 0;
    printf("PC: %04X, AF: %02X%02X, BC: %02X%02X, DE: %02X%02X, HL: %02X%02X, SP: %04X\n", state->pc, state->a, f, state->b, state->c, state->d, state->e,
           state->h, state->l, state->sp);
}

/*
   *codebuffer is a valid pointer to 8080 assembly code
   pc is the current offset into the code
   returns the number of bytes of the op
  */
int Disassemble8080Op(unsigned char *codebuffer, int pc) {
    unsigned char *code = &codebuffer[pc];
    int opbytes = 1;
    printf("%04x ", pc);
    switch (*code) {
    case 0x00:
        printf("NOP"); //
        break;
    case 0x01:
        printf("LXI B,#$%02x%02x", code[2], code[1]); //
        opbytes = 3;
        break;
    case 0x02:
        printf("STAX B");
        break;
    case 0x03:
        printf("INX B");
        break;
    case 0x04:
        printf("INR B");
        break;
    case 0x05:
        printf("DCR B");
        break;
    case 0x06:
        printf("MVI B,#$%02x", code[1]);
        opbytes = 2;
        break;
    case 0x07:
        printf("RLC");
        break;
    case 0x08:
        printf("NOP");
        break;
    case 0x09:
        printf("DAD B");
        break;
    case 0x0a:
        printf("LDAX B");
        break;
    case 0x0b:
        printf("DCX B");
        break;
    case 0x0c:
        printf("INR C");
        break;
    case 0x0d:
        printf("DCR C");
        break;
    case 0x0e:
        printf("MVI C,#$%02x", code[1]);
        opbytes = 2;
        break;
    case 0x0f:
        printf("RRC");
        break;
    case 0x10:
        printf("NOP");
        break;
    case 0x11:
        printf("LXI D,#$%02x%02x", code[2], code[1]);
        opbytes = 3;
        break;
    case 0x12:
        printf("STAX D");
        break;
    case 0x13:
        printf("INX D");
        break;
    case 0x14:
        printf("INR D");
        break;
    case 0x15:
        printf("DCR D");
        break;
    case 0x16:
        printf("MVI D,#$%02x", code[1]);
        opbytes = 2;
        break;
    case 0x17:
        printf("RAL");
        break;
    case 0x18:
        printf("NOP");
        break;
    case 0x19:
        printf("DAD D");
        break;
    case 0x1a:
        printf("LDAX D");
        break;
    case 0x1b:
        printf("DCX D");
        break;
    case 0x1c:
        printf("INR E");
        break;
    case 0x1d:
        printf("DCR E");
        break;
    case 0x1e:
        printf("MVI E,#$%02x", code[1]);
        opbytes = 2;
        break;
    case 0x1f:
        printf("RAR");
        break;
    case 0x20:
        printf("NOP");
        break;
    case 0x21:
        printf("LXI H,#$%02x%02x", code[2], code[1]);
        opbytes = 3;
        break;
    case 0x22:
        printf("SHLD $%02x%02x", code[2], code[1]);
        opbytes = 3;
        break;
    case 0x23:
        printf("INX H");
        break;
    case 0x24:
        printf("INR H");
        break;
    case 0x25:
        printf("DCR H");
        break;
    case 0x26:
        printf("MVI H,#$%02x", code[1]);
        opbytes = 2;
        break;
    case 0x27:
        printf("DAA");
        break;
    case 0x28:
        printf("NOP");
        break;
    case 0x29:
        printf("DAD H");
        break;
    case 0x2a:
        printf("LHLD $%02x%02x", code[2], code[1]);
        opbytes = 3;
        break;
    case 0x2b:
        printf("DCX H");
        break;
    case 0x2c:
        printf("INR L");
        break;
    case 0x2d:
        printf("DCR L");
        break;
    case 0x2e:
        printf("MVI L,#$%02x", code[1]);
        opbytes = 2;
        break;
    case 0x2f:
        printf("CMA");
        break;
    case 0x30:
        printf("NOP");
        break;
    case 0x31:
        printf("LXI SP,#$%02x%02x", code[2], code[1]);
        opbytes = 3;
        break;
    case 0x32:
        printf("STA $%02x%02x", code[2], code[1]);
        opbytes = 3;
        break;
    case 0x33:
        printf("INX SP");
        break;
    case 0x34:
        printf("INR M");
        break;
    case 0x35:
        printf("DCR M");
        break;
    case 0x36:
        printf("MVI M,#$%02x", code[1]);
        opbytes = 2;
        break;
    case 0x37:
        printf("STC");
        break;
    case 0x38:
        printf("NOP");
        break;
    case 0x39:
        printf("DAD SP");
        break;
    case 0x3a:
        printf("LDA $%02x%02x", code[2], code[1]);
        opbytes = 3;
        break;
    case 0x3b:
        printf("DCX SP");
        break;
    case 0x3c:
        printf("INR A");
        break;
    case 0x3d:
        printf("DCR A");
        break;
    case 0x3e:
        printf("MVI A,#$%02x", code[1]);
        opbytes = 2;
        break;
    case 0x3f:
        printf("CMC");
        break;
    case 0x40:
        printf("MOV B,B");
        break;
    case 0x41:
        printf("MOV B,C");
        break;
    case 0x42:
        printf("MOV B,D");
        break;
    case 0x43:
        printf("MOV B,E");
        break;
    case 0x44:
        printf("MOV B,H");
        break;
    case 0x45:
        printf("MOV B,L");
        break;
    case 0x46:
        printf("MOV B,M");
        break;
    case 0x47:
        printf("MOV B,A");
        break;
    case 0x48:
        printf("MOV C,B");
        break;
    case 0x49:
        printf("MOV C,C");
        break;
    case 0x4a:
        printf("MOV C,D");
        break;
    case 0x4b:
        printf("MOV C,E");
        break;
    case 0x4c:
        printf("MOV C,H");
        break;
    case 0x4d:
        printf("MOV C,L");
        break;
    case 0x4e:
        printf("MOV C,M");
        break;
    case 0x4f:
        printf("MOV C,A");
        break;
    case 0x50:
        printf("MOV D,B");
        break;
    case 0x51:
        printf("MOV D,C");
        break;
    case 0x52:
        printf("MOV D,D");
        break;
    case 0x53:
        printf("MOV D,E");
        break;
    case 0x54:
        printf("MOV D,H");
        break;
    case 0x55:
        printf("MOV D,L");
        break;
    case 0x56:
        printf("MOV D,M");
        break;
    case 0x57:
        printf("MOV D,A");
        break;
    case 0x58:
        printf("MOV E,B");
        break;
    case 0x59:
        printf("MOV E,C");
        break;
    case 0x5a:
        printf("MOV E,D");
        break;
    case 0x5b:
        printf("MOV E,E");
        break;
    case 0x5c:
        printf("MOV E,H");
        break;
    case 0x5d:
        printf("MOV E,L");
        break;
    case 0x5e:
        printf("MOV E,M");
        break;
    case 0x5f:
        printf("MOV E,A");
        break;
    case 0x60:
        printf("MOV H,B");
        break;
    case 0x61:
        printf("MOV H,C");
        break;
    case 0x62:
        printf("MOV H,D");
        break;
    case 0x63:
        printf("MOV H,E");
        break;
    case 0x64:
        printf("MOV H,H");
        break;
    case 0x65:
        printf("MOV H,L");
        break;
    case 0x66:
        printf("MOV H,M");
        break;
    case 0x67:
        printf("MOV H,A");
        break;
    case 0x68:
        printf("MOV L,B");
        break;
    case 0x69:
        printf("MOV L,C");
        break;
    case 0x6a:
        printf("MOV L,D");
        break;
    case 0x6b:
        printf("MOV L,E");
        break;
    case 0x6c:
        printf("MOV L,H");
        break;
    case 0x6d:
        printf("MOV L,L");
        break;
    case 0x6e:
        printf("MOV L,M");
        break;
    case 0x6f:
        printf("MOV L,A");
        break;
    case 0x70:
        printf("MOV M,B");
        break;
    case 0x71:
        printf("MOV M,C");
        break;
    case 0x72:
        printf("MOV M,D");
        break;
    case 0x73:
        printf("MOV M,E");
        break;
    case 0x74:
        printf("MOV M,H");
        break;
    case 0x75:
        printf("MOV M,L");
        break;
    case 0x76:
        printf("MOV M,M");
        break;
    case 0x77:
        printf("MOV M,A");
        break;
    case 0x78:
        printf("MOV A,B");
        break;
    case 0x79:
        printf("MOV A,C");
        break;
    case 0x7a:
        printf("MOV A,D");
        break;
    case 0x7b:
        printf("MOV A,E");
        break;
    case 0x7c:
        printf("MOV A,H");
        break;
    case 0x7d:
        printf("MOV A,L");
        break;
    case 0x7e:
        printf("MOV A,M");
        break;
    case 0x7f:
        printf("MOV A,A");
        break;
    case 0x80:
        printf("ADD B");
        break;
    case 0x81:
        printf("ADD C");
        break;
    case 0x82:
        printf("ADD D");
        break;
    case 0x83:
        printf("ADD E");
        break;
    case 0x84:
        printf("ADD H");
        break;
    case 0x85:
        printf("ADD L");
        break;
    case 0x86:
        printf("ADD M");
        break;
    case 0x87:
        printf("ADD A");
        break;
    case 0x88:
        printf("ADC B");
        break;
    case 0x89:
        printf("ADC C");
        break;
    case 0x8a:
        printf("ADC D");
        break;
    case 0x8b:
        printf("ADC E");
        break;
    case 0x8c:
        printf("ADC H");
        break;
    case 0x8d:
        printf("ADC L");
        break;
    case 0x8e:
        printf("ADC M");
        break;
    case 0x8f:
        printf("ADC A");
        break;
    case 0x90:
        printf("SUB B");
        break;
    case 0x91:
        printf("SUB C");
        break;
    case 0x92:
        printf("SUB D");
        break;
    case 0x93:
        printf("SUB E");
        break;
    case 0x94:
        printf("SUB H");
        break;
    case 0x95:
        printf("SUB L");
        break;
    case 0x96:
        printf("SUB M");
        break;
    case 0x97:
        printf("SUB A");
        break;
    case 0x98:
        printf("SBB B");
        break;
    case 0x99:
        printf("SBB C");
        break;
    case 0x9a:
        printf("SBB D");
        break;
    case 0x9b:
        printf("SBB E");
        break;
    case 0x9c:
        printf("SBB H");
        break;
    case 0x9d:
        printf("SBB L");
        break;
    case 0x9e:
        printf("SBB M");
        break;
    case 0x9f:
        printf("SBB A");
        break;
    case 0xa0:
        printf("ANA B");
        break;
    case 0xa1:
        printf("ANA C");
        break;
    case 0xa2:
        printf("ANA D");
        break;
    case 0xa3:
        printf("ANA E");
        break;
    case 0xa4:
        printf("ANA H");
        break;
    case 0xa5:
        printf("ANA L");
        break;
    case 0xa6:
        printf("ANA M");
        break;
    case 0xa7:
        printf("ANA A");
        break;
    case 0xa8:
        printf("XRA B");
        break;
    case 0xa9:
        printf("XRA C");
        break;
    case 0xaa:
        printf("XRA D");
        break;
    case 0xab:
        printf("XRA E");
        break;
    case 0xac:
        printf("XRA H");
        break;
    case 0xad:
        printf("XRA L");
        break;
    case 0xae:
        printf("XRA M");
        break;
    case 0xaf:
        printf("XRA A");
        break;
    case 0xb0:
        printf("ORA B");
        break;
    case 0xb1:
        printf("ORA C");
        break;
    case 0xb2:
        printf("ORA D");
        break;
    case 0xb3:
        printf("ORA E");
        break;
    case 0xb4:
        printf("ORA H");
        break;
    case 0xb5:
        printf("ORA L");
        break;
    case 0xb6:
        printf("ORA M");
        break;
    case 0xb7:
        printf("ORA A");
        break;
    case 0xb8:
        printf("CMP B");
        break;
    case 0xb9:
        printf("CMP C");
        break;
    case 0xba:
        printf("CMP D");
        break;
    case 0xbb:
        printf("CMP E");
        break;
    case 0xbc:
        printf("CMP H");
        break;
    case 0xbd:
        printf("CMP L");
        break;
    case 0xbe:
        printf("CMP M");
        break;
    case 0xbf:
        printf("CMP A");
        break;
    case 0xc0:
        printf("RNZ");
        break;
    case 0xc1:
        printf("POP B");
        break;
    case 0xc2:
        printf("JNZ $%02x%02x", code[2], code[1]);
        opbytes = 3;
        break;
    case 0xc3:
        printf("JMP $%02x%02x", code[2], code[1]);
        opbytes = 3;
        break;
    case 0xc4:
        printf("CNZ $%02x%02x", code[2], code[1]);
        opbytes = 3;
        break;
    case 0xc5:
        printf("PUSH B");
        break;
    case 0xc6:
        printf("ADI #$%02x", code[1]);
        opbytes = 2;
        break;
    case 0xc7:
        printf("RST 0");
        break;
    case 0xc8:
        printf("RZ");
        break;
    case 0xc9:
        printf("RET");
        break;
    case 0xca:
        printf("JZ $%02x%02x", code[2], code[1]);
        opbytes = 3;
        break;
    case 0xcb:
        printf("NOP");
        break;
    case 0xcc:
        printf("CZ $%02x%02x", code[2], code[1]);
        opbytes = 3;
        break;
    case 0xcd:
        printf("CALL $%02x%02x", code[2], code[1]);
        opbytes = 3;
        break;
    case 0xce:
        printf("ACI #$%02x", code[1]);
        opbytes = 2;
        break;
    case 0xcf:
        printf("RST 1");
        break;
    case 0xd0:
        printf("RNC");
        break;
    case 0xd1:
        printf("POP D");
        break;
    case 0xd2:
        printf("JNC $%02x%02x", code[2], code[1]);
        opbytes = 3;
        break;
    case 0xd3:
        printf("OUT #$%02x", code[1]);
        opbytes = 2;
        break;
    case 0xd4:
        printf("CNC $%02x%02x", code[2], code[1]);
        opbytes = 3;
        break;
    case 0xd5:
        printf("PUSH D");
        break;
    case 0xd6:
        printf("SUI #$%02x", code[1]);
        opbytes = 2;
        break;
    case 0xd7:
        printf("RST 2");
        break;
    case 0xd8:
        printf("RC");
        break;
    case 0xd9:
        printf("NOP");
        break;
    case 0xda:
        printf("JC $%02x%02x", code[2], code[1]);
        opbytes = 3;
        break;
    case 0xdb:
        printf("IN #$%02x", code[1]);
        opbytes = 2;
        break;
    case 0xdc:
        printf("CC $%02x%02x", code[2], code[1]);
        opbytes = 3;
        break;
    case 0xdd:
        printf("NOP");
        break;
    case 0xde:
        printf("SBI #$%02x", code[1]);
        opbytes = 2;
        break;
    case 0xdf:
        printf("RST 3");
        break;
    case 0xe0:
        printf("RPO");
        break;
    case 0xe1:
        printf("POP H");
        break;
    case 0xe2:
        printf("JPO $%02x%02x", code[2], code[1]);
        opbytes = 3;
        break;
    case 0xe3:
        printf("XTHL");
        break;
    case 0xe4:
        printf("CPO $%02x%02x", code[2], code[1]);
        opbytes = 3;
        break;
    case 0xe5:
        printf("PUSH H");
        break;
    case 0xe6:
        printf("ANI #$%02x", code[1]);
        opbytes = 2;
        break;
    case 0xe7:
        printf("RST 4");
        break;
    case 0xe8:
        printf("RPE");
        break;
    case 0xe9:
        printf("PCHL");
        break;
    case 0xea:
        printf("JPE $%02x%02x", code[2], code[1]);
        opbytes = 3;
        break;
    case 0xeb:
        printf("XCHG");
        break;
    case 0xec:
        printf("CPE $%02x%02x", code[2], code[1]);
        opbytes = 3;
        break;
    case 0xed:
        printf("NOP");
        break;
    case 0xee:
        printf("XRI #$%02x", code[1]);
        opbytes = 2;
        break;
    case 0xef:
        printf("RST 5");
        break;
    case 0xf0:
        printf("RP");
        break;
    case 0xf1:
        printf("POP PSW");
        break;
    case 0xf2:
        printf("JP $%02x%02x", code[2], code[1]);
        opbytes = 3;
        break;
    case 0xf3:
        printf("DI");
        break;
    case 0xf4:
        printf("CP $%02x%02x", code[2], code[1]);
        opbytes = 3;
        break;
    case 0xf5:
        printf("PUSH PSW");
        break;
    case 0xf6:
        printf("ORI #$%02x", code[1]);
        opbytes = 2;
        break;
    case 0xf7:
        printf("RST 6");
        break;
    case 0xf8:
        printf("RM");
        break;
    case 0xf9:
        printf("SPHL");
        break;
    case 0xfa:
        printf("JM $%02x%02x", code[2], code[1]);
        opbytes = 3;
        break;
    case 0xfb:
        printf("EI");
        break;
    case 0xfc:
        printf("CM $%02x%02x", code[2], code[1]);
        opbytes = 3;
        break;
    case 0xfd:
        printf("NOP");
        break;
    case 0xfe:
        printf("CPI #$%02x", code[1]);
        opbytes = 2;
        break;
    case 0xff:
        printf("RST 7");
        break;
    }
    printf(" %02x\n", code[0]);

    return opbytes;
}

static void UnimplementedInstruction(State8080 *state) {
    printf("Error: Unimplemented instruction\n");
    printf("iteration: %d\n", state->iteration_number - 1);
    print_state(state);
    printf("\n");
    Disassemble8080Op(state->memory, state->pc - 1); // make sure to decrease pc
    exit(1);
}

static inline _Bool Parity(uint16_t x) {
    uint16_t sum = 0;

    for (sum = 0; x > 0; x = (x >> 1)) {
        sum += ((x & 0x0001) == 0x0001);
    }
    return (sum % 2 == 0);
}

static inline uint16_t opcode_data(unsigned char *opcode) {
    return (opcode[2] << 8) | opcode[1];
}

// TODO: understand this function
static inline bool AuxiliaryCarry(uint16_t answer, uint8_t a, uint8_t b) {
    int16_t carry = answer ^ a ^ b;
    return carry & (1 << 4);
}

void Emulate8080Op(State8080 *state) {
    unsigned char *opcode = &state->memory[state->pc];

    state->iteration_number += 1;
    state->pc += 1; // for the opcode

    // When EI is executed, the interrupt system is enabled following the execution of the next instruction.
    // See how EI is implemented
    if (state->int_delay > 0) {
        state->int_delay -= 1;
    }

    switch (*opcode) {
    case 0x00: // NOP
        break;
    case 0x01: // LXI B, data 16
        state->b = opcode[2];
        state->c = opcode[1];
        state->pc += 2;
        break;
    case 0x02: // STAX B
    {
        uint16_t addr = (state->b << 8) | state->c;
        state->memory[addr] = state->a;
    } break;
    case 0x03: // INX B
    {
        uint32_t answer = ((state->b << 8) | state->c) + 1;
        state->b = (answer >> 8) & 0xff;
        state->c = answer & 0xff;
    } break;
    case 0x04: // INR B
    {
        uint16_t answer = state->b + 1;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = AuxiliaryCarry(answer, state->b, 1);
        state->b = answer;
    } break;
    case 0x05: // DCR B
    {
        uint8_t answer = state->b - 1;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = AuxiliaryCarry(answer, state->b, ~1);
        state->b = answer;
    } break;
    case 0x06: // MVI B, data
        state->b = opcode[1];
        state->pc += 1;
        break;
    case 0x07: // RLC
    {
        uint8_t a = state->a;
        state->cc.cy = ((a >> 7) & 1) == 1;
        state->a = (a << 1) | ((a >> 7) & 1);
    } break;
    case 0x08: // NOP
        break;
    case 0x09: // DAD B
    {
        uint32_t answer = (state->h << 8) | state->l;
        answer += (state->b << 8) | state->c;
        state->cc.cy = (answer & 0xffff0000) != 0;
        state->h = (answer & 0xff00) >> 8;
        state->l = answer & 0xff;
    } break;
    case 0x0a: // LDAX B
    {
        uint16_t addr = (state->b << 8) | state->c;
        state->a = state->memory[addr];
    } break;
        break;
    case 0x0b: // DCX B
    {
        uint32_t answer = ((state->b << 8) | state->c) - 1;
        state->b = (answer >> 8) & 0xff;
        state->c = answer & 0xff;
    } break;
    case 0x0c: // INR C
    {
        uint16_t answer = state->c + 1;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = AuxiliaryCarry(answer, state->c, 1);
        state->c = answer;
    } break;
    case 0x0d: // DCR C
    {
        uint8_t answer = state->c - 1;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = AuxiliaryCarry(answer, state->c, ~1);
        state->c = answer;
    } break;
    case 0x0e: // MVI C, data
        state->c = opcode[1];
        state->pc += 1;
        break;
    case 0x0f: // RRC
    {
        uint8_t a = state->a;
        state->cc.cy = ((a & 1) == 1);
        state->a = (a >> 1) | ((a & 1) << 7);
    } break;
    case 0x10: // NOP
        break;
    case 0x11: // LXI D, data 16
        state->d = opcode[2];
        state->e = opcode[1];
        state->pc += 2;
        break;
    case 0x12: // STAX D
    {
        uint16_t addr = (state->d << 8) | state->e;
        state->memory[addr] = state->a;
    } break;
    case 0x13: // INX D
        // NOTE: Do nothing on overflow
        {
            /* uint32_t answer = ((state->d << 8) | state->e) + 1; */
            /* state->d = (answer & 0xff00) >> 8; */
            /* state->e = answer & 0xff; */
            state->e++;
            if (state->e == 0) {
                state->d++;
            }
        }
        break;
    case 0x14: // INR D
    {
        uint16_t answer = state->d + 1;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = AuxiliaryCarry(answer, state->d, 1);
        state->d = answer;
    } break;
    case 0x15: // DCR D
    {
        uint8_t answer = state->d - 1;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = AuxiliaryCarry(answer, state->d, ~1);
        state->d = answer;
    } break;
    case 0x16: // MVI D, data
        state->d = opcode[1];
        state->pc += 1;
        break;
    case 0x17: // RAL
    {
        uint8_t a = state->a;
        state->a = (a << 1) | (state->cc.cy & 1);
        state->cc.cy = ((a >> 7) & 1) == 1;
    } break;
    case 0x18: // NOP
        break;
    case 0x19: // DAD D
    {
        uint32_t answer = (state->h << 8) | state->l;
        answer += (state->d << 8) | state->e;
        state->cc.cy = (answer & 0xffff0000) != 0;
        state->h = (answer & 0xff00) >> 8;
        state->l = answer & 0xff;
    } break;
    case 0x1a: // LDAX D
    {
        uint16_t addr = (state->d << 8) | state->e;
        state->a = state->memory[addr];
    } break;
    case 0x1b: // DCX D
    {
        uint32_t answer = ((state->d << 8) | state->e) - 1;
        state->d = (answer >> 8) & 0xff;
        state->e = answer & 0xff;
    } break;
    case 0x1c: // INR E
    {
        uint16_t answer = state->e + 1;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = AuxiliaryCarry(answer, state->e, 1);
        state->e = answer;
    } break;
    case 0x1d: // DCR E
    {
        uint8_t answer = state->e - 1;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = AuxiliaryCarry(answer, state->e, ~1);
        state->e = answer;
    } break;
    case 0x1e: // MVI E,data
        state->e = opcode[1];
        state->pc += 1;
        break;
    case 0x1f: // RAR
    {
        uint8_t a = state->a;
        state->a = (a >> 1) | (state->cc.cy << 7);
        state->cc.cy = ((a & 1) == 1);
    } break;
    case 0x20: // NOP
        break;
    case 0x21: // LXI H, data 16
        state->h = opcode[2];
        state->l = opcode[1];
        state->pc += 2;
        break;
    case 0x22: // SHLD data 16
    {
        uint16_t addr = opcode_data(opcode);
        state->memory[addr] = state->l;
        state->memory[addr + 1] = state->h;
        state->pc += 2;
    } break;
    case 0x23: // INX H
        // NOTE: Do nothing on overflow
        {
            /* uint32_t answer = ((state->h << 8) | state->l) + 1; */
            /* state->h = (answer & 0xff00) >> 8; */
            /* state->l = answer & 0xff; */
            state->l++;
            if (state->l == 0) {
                state->h++;
            }
        }
        break;
    case 0x24: // INR H
    {
        uint16_t answer = state->h + 1;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = AuxiliaryCarry(answer, state->h, 1);
        state->h = answer;
    } break;
    case 0x25: // DCR H
    {
        uint8_t answer = state->h - 1;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = AuxiliaryCarry(answer, state->h, ~1);
        state->h = answer;
    } break;
    case 0x26: // MVI H, data
        state->h = opcode[1];
        state->pc += 1;
        break;
    case 0x27: // DAA
    {
        uint16_t answer = state->a;
        uint16_t added = 0x00;
        uint8_t cy = state->cc.cy;
        if (state->cc.ac || ((answer & 0x0f) > 0x09)) {
            answer += 0x06;
            added += 0x06;
        }
        if (state->cc.cy || ((answer & 0xf0) > 0x90)) {
            answer += 0x60;
            added += 0x60;
            cy = 1;
        }
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.p = Parity(answer & 0xff);
        state->cc.cy = cy;
        /* state->cc.cy = (answer > 0xff); */
        state->cc.ac = AuxiliaryCarry(answer, state->a, added);
        state->a = answer & 0xff;
    } break;
    case 0x28: // NOP
        break;
    case 0x29: // DAD H
    {
        uint32_t answer = (state->h << 8) | state->l;
        answer += answer;
        state->cc.cy = (answer & 0xffff0000) != 0;
        state->h = (answer & 0xff00) >> 8;
        state->l = answer & 0xff;
    } break;
    case 0x2a: // LHLD data 16
    {
        uint16_t addr = opcode_data(opcode);
        state->l = state->memory[addr];
        state->h = state->memory[addr + 1];
        state->pc += 2;
    } break;
    case 0x2b: // DCX H
    {
        uint32_t answer = ((state->h << 8) | state->l) - 1;
        state->h = (answer >> 8) & 0xff;
        state->l = answer & 0xff;
    } break;
    case 0x2c: // INR L
    {
        uint16_t answer = state->l + 1;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = AuxiliaryCarry(answer, state->l, 1);
        state->l = answer;
    } break;
    case 0x2d: // DCR L
    {
        uint8_t answer = state->l - 1;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = AuxiliaryCarry(answer, state->l, ~1);
        state->l = answer;
    } break;
    case 0x2e: // MVI L,data
        state->l = opcode[1];
        state->pc += 1;
        break;
    case 0x2f: // CMA
        state->a = ~state->a;
        break;
    case 0x30: // NOP
        break;
    case 0x31: // LXI SP, data 16
        state->sp = opcode_data(opcode);
        state->pc += 2;
        break;
    case 0x32: // STA Addr
    {
        uint16_t addr = (opcode[2] << 8) | opcode[1];
        state->memory[addr] = state->a;
        state->pc += 2;
    } break;
    case 0x33: // INX SP
        state->sp += 1;
        break;
    case 0x34: // INR M
    {
        uint16_t addr = (state->h << 8) | state->l;
        uint16_t answer = state->memory[addr] + 1;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = AuxiliaryCarry(answer, state->memory[addr], 1);
        state->memory[addr] = answer;
    } break;
    case 0x35: // DCR M
    {
        uint16_t addr = (state->h << 8) | state->l;
        uint8_t answer = state->memory[addr] - 1;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = AuxiliaryCarry(answer, state->memory[addr], 0xfe);
        state->memory[addr] -= 1;
    } break;
    case 0x36: // MVI M, data
    {
        uint16_t addr = (state->h << 8) | state->l;
        state->memory[addr] = opcode[1];
        state->pc += 1;
    } break;
    case 0x37: // STC
        state->cc.cy = 1;
        break;
    case 0x38: // NOP
        break;
    case 0x39: // DAD SP
    {
        uint32_t answer = (state->h << 8) | state->l;
        answer += state->sp;
        state->cc.cy = (answer & 0xffff0000) != 0;
        state->h = (answer & 0xff00) >> 8;
        state->l = answer & 0xff;
    } break;
    case 0x3a: // LDA data 16
    {
        uint16_t addr = (opcode[2] << 8) | opcode[1];
        state->a = state->memory[addr];
        state->pc += 2;
    } break;
    case 0x3b: // DCX SP
        state->sp -= 1;
        break;
    case 0x3c: // INR A
    {
        uint16_t answer = state->a + 1;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = AuxiliaryCarry(answer, state->a, 1);
        state->a = answer;
    } break;
    case 0x3d: // DCR A
    {
        uint8_t answer = state->a - 1;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = AuxiliaryCarry(answer, state->a, 0xfe);
        state->a = answer;
    } break;
    case 0x3e: // MVI A, data
        state->a = opcode[1];
        state->pc += 1;
        break;
    case 0x3f: // CMC
        state->cc.cy = (state->cc.cy == 0);
        break;
    case 0x40: // MOV B,B
        break;
    case 0x41: // MOV B,C
        state->b = state->c;
        break;
    case 0x42: // MOV B,D
        state->b = state->d;
        break;
    case 0x43: // MOV B,E
        state->b = state->e;
        break;
    case 0x44: // MOV B,H
        state->b = state->h;
        break;
    case 0x45: // MOV B,L
        state->b = state->l;
        break;
    case 0x46: // MOV B,M
    {
        uint16_t addr = (state->h << 8) | state->l;
        state->b = state->memory[addr];
    } break;
        break;
    case 0x47: // MOV B,A
        state->b = state->a;
        break;
    case 0x48: // MOV C,B
        state->c = state->b;
        break;
    case 0x49: // MOV C,C
        break;
    case 0x4a: // MOV C,D
        state->c = state->d;
        break;
    case 0x4b: // MOV C,E
        state->c = state->e;
        break;
    case 0x4c: // MOV C,H
        state->c = state->h;
        break;
    case 0x4d: // MOV C,L
        state->c = state->l;
        break;
    case 0x4e: // MOV C,M
    {
        uint16_t addr = (state->h << 8) | state->l;
        state->c = state->memory[addr];
    } break;
        break;
    case 0x4f: // MOV C,A
        state->c = state->a;
        break;
    case 0x50: // MOV D,B
        state->d = state->b;
        break;
    case 0x51: // MOV D,C
        state->d = state->c;
        break;
    case 0x52: // MOV D,D
        break;
    case 0x53: // MOV D,E
        state->d = state->e;
        break;
    case 0x54: // MOV D,H
        state->d = state->h;
        break;
    case 0x55: // MOV D,L
        state->d = state->l;
        break;
    case 0x56: // MOV D,M
    {
        uint16_t addr = (state->h << 8) | state->l;
        state->d = state->memory[addr];
    } break;
    case 0x57: // MOV D,A
        state->d = state->a;
        break;
    case 0x58: // MOV E,B
        state->e = state->b;
        break;
    case 0x59: // MOV E,C
        state->e = state->c;
        break;
    case 0x5a: // MOV E,D
        state->e = state->d;
        break;
    case 0x5b: // MOV E,E
        break;
    case 0x5c: // MOV E,H
        state->e = state->h;
        break;
    case 0x5d: // MOV E,L
        state->e = state->l;
        break;
    case 0x5e: // MOV E,M
    {
        uint16_t addr = (state->h << 8) | state->l;
        state->e = state->memory[addr];
    } break;
    case 0x5f: // MOV E,A
        state->e = state->a;
        break;
    case 0x60: // MOV H,B
        state->h = state->b;
        break;
    case 0x61: // MOV H,C
        state->h = state->c;
        break;
    case 0x62: // MOV H,D
        state->h = state->d;
        break;
    case 0x63: // MOV H,E
        state->h = state->e;
        break;
    case 0x64: // MOV H,H
        break;
    case 0x65: // MOV H,L
        state->h = state->l;
        break;
    case 0x66: // MOV H,M
    {
        uint16_t addr = (state->h << 8) | state->l;
        state->h = state->memory[addr];
    } break;
    case 0x67: // MOV H,A
        state->h = state->a;
        break;
    case 0x68: // MOV L,B
        state->l = state->b;
        break;
    case 0x69: // MOV L,C
        state->l = state->c;
        break;
    case 0x6a: // MOV L,D
        state->l = state->d;
        break;
    case 0x6b: // MOV L,E
        state->l = state->e;
        break;
    case 0x6c: // MOV L,H
        state->l = state->h;
        break;
    case 0x6d: // MOV L,L
        break;
    case 0x6e: // MOV L,M
    {
        uint16_t addr = (state->h << 8) | state->l;
        state->l = state->memory[addr];
    } break;
    case 0x6f: // MOV L,A
        state->l = state->a;
        break;
    case 0x70: // MOV M,B
    {
        uint16_t addr = (state->h << 8) | state->l;
        state->memory[addr] = state->b;
    } break;
    case 0x71: // MOV M,C
    {
        uint16_t addr = (state->h << 8) | state->l;
        state->memory[addr] = state->c;
    } break;
    case 0x72: // MOV M,D
    {
        uint16_t addr = (state->h << 8) | state->l;
        state->memory[addr] = state->d;
    } break;
    case 0x73: // MOV M,E
    {
        uint16_t addr = (state->h << 8) | state->l;
        state->memory[addr] = state->e;
    } break;
    case 0x74: // MOV M,H
    {
        uint16_t addr = (state->h << 8) | state->l;
        state->memory[addr] = state->h;
    } break;
    case 0x75: // MOV M,L
    {
        uint16_t addr = (state->h << 8) | state->l;
        state->memory[addr] = state->l;
    } break;
    case 0x76: // MOV M,M
        break;
    case 0x77: // MOV M,A
    {
        uint16_t addr = (state->h << 8) | state->l;
        state->memory[addr] = state->a;
    } break;
    case 0x78: // MOV A,B
        state->a = state->b;
        break;
    case 0x79: // MOV A,C
        state->a = state->c;
        break;
    case 0x7a: // MOV A,D
        state->a = state->d;
        break;
    case 0x7b: // MOV A,E
        state->a = state->e;
        break;
    case 0x7c: // MOV A,H
        state->a = state->h;
        break;
    case 0x7d: // MOV A,L
        state->a = state->l;
        break;
    case 0x7e: // MOV A,M
    {
        uint16_t addr = (state->h << 8) | state->l;
        state->a = state->memory[addr];
    } break;
    case 0x7f: // MOV A,A
        break;
    case 0x80: // ADD B
    {
        uint16_t answer = (uint16_t)state->a + (uint16_t)state->b;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.cy = (answer > 0xff);
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = AuxiliaryCarry(answer, state->a, state->b);
        state->a = answer & 0xff;
    } break;
    case 0x81: // ADD C
    {
        uint16_t answer = (uint16_t)state->a + (uint16_t)state->c;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.cy = (answer > 0xff);
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = AuxiliaryCarry(answer, state->a, state->c);
        state->a = answer & 0xff;
    } break;
    case 0x82: // ADD D
    {
        uint16_t answer = (uint16_t)state->a + (uint16_t)state->d;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.cy = (answer > 0xff);
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = AuxiliaryCarry(answer, state->a, state->d);
        state->a = answer & 0xff;
    } break;
    case 0x83: // ADD E
    {
        uint16_t answer = (uint16_t)state->a + (uint16_t)state->e;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.cy = (answer > 0xff);
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = AuxiliaryCarry(answer, state->a, state->e);
        state->a = answer & 0xff;
    } break;
    case 0x84: // ADD H
    {
        uint16_t answer = (uint16_t)state->a + (uint16_t)state->h;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.cy = (answer > 0xff);
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = AuxiliaryCarry(answer, state->a, state->h);
        state->a = answer & 0xff;
    } break;
    case 0x85: // ADD L
    {
        uint16_t answer = (uint16_t)state->a + (uint16_t)state->l;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.cy = (answer > 0xff);
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = AuxiliaryCarry(answer, state->a, state->l);
        state->a = answer & 0xff;
    } break;
    case 0x86: // ADD M
    {
        uint16_t offset = (state->h << 8) | (state->l);
        uint16_t answer = (uint16_t)state->a + state->memory[offset];
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.cy = (answer > 0xff);
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = AuxiliaryCarry(answer, state->a, state->memory[offset]);
        state->a = answer & 0xff;
    } break;
    case 0x87: // ADD A
    {
        uint16_t answer = (uint16_t)state->a + (uint16_t)state->a;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.cy = (answer > 0xff);
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = AuxiliaryCarry(answer, state->a, state->a);
        state->a = answer & 0xff;
    } break;
    case 0x88: // ADC B
    {
        uint16_t answer = (uint16_t)state->a + (uint16_t)state->b + state->cc.cy;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.cy = (answer > 0xff);
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = AuxiliaryCarry(answer, state->a, state->b + state->cc.cy);
        state->a = answer & 0xff;
    } break;
    case 0x89: // ADC C
    {
        uint16_t answer = (uint16_t)state->a + (uint16_t)state->c + state->cc.cy;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.cy = (answer > 0xff);
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = AuxiliaryCarry(answer, state->a, state->c + state->cc.cy);
        state->a = answer & 0xff;
    } break;
    case 0x8a: // ADC D
    {
        uint16_t answer = (uint16_t)state->a + (uint16_t)state->d + state->cc.cy;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.cy = (answer > 0xff);
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = AuxiliaryCarry(answer, state->a, state->d + state->cc.cy);
        state->a = answer & 0xff;
    } break;
    case 0x8b: // ADC E
    {
        uint16_t answer = (uint16_t)state->a + (uint16_t)state->e + state->cc.cy;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.cy = (answer > 0xff);
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = AuxiliaryCarry(answer, state->a, state->e + state->cc.cy);
        state->a = answer & 0xff;
    } break;
    case 0x8c: // ADC H
    {
        uint16_t answer = (uint16_t)state->a + (uint16_t)state->h + state->cc.cy;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.cy = (answer > 0xff);
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = AuxiliaryCarry(answer, state->a, state->h + state->cc.cy);
        state->a = answer & 0xff;
    } break;
    case 0x8d: // ADC L
    {
        uint16_t answer = (uint16_t)state->a + (uint16_t)state->l + state->cc.cy;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.cy = (answer > 0xff);
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = AuxiliaryCarry(answer, state->a, state->l + state->cc.cy);
        state->a = answer & 0xff;
    } break;
    case 0x8e: // ADC M
    {
        uint16_t addr = (state->h << 8) | state->l;
        uint16_t answer = (uint16_t)state->a + state->memory[addr] + state->cc.cy;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.cy = (answer > 0xff);
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = AuxiliaryCarry(answer, state->a, state->memory[addr] + state->cc.cy);
        state->a = answer & 0xff;
    } break;
    case 0x8f: // ADC A
    {
        uint16_t answer = (uint16_t)state->a + (uint16_t)state->a + state->cc.cy;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.cy = (answer > 0xff);
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = AuxiliaryCarry(answer, state->a, state->a + state->cc.cy);
        state->a = answer & 0xff;
    } break;
    case 0x90: // SUB B
    {
        uint16_t answer = state->a - state->b;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.p = Parity(answer & 0xff);
        state->cc.cy = (answer > 0xff);
        state->cc.ac = AuxiliaryCarry(answer, state->a, ~state->b);
        state->a = answer & 0xff;
    } break;
    case 0x91: // SUB C
    {
        uint16_t answer = state->a - state->c;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.p = Parity(answer & 0xff);
        state->cc.cy = (answer > 0xff);
        state->cc.ac = AuxiliaryCarry(answer, state->a, ~state->c);
        state->a = answer & 0xff;
    } break;
    case 0x92: // SUB D
    {
        uint16_t answer = state->a - state->d;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.p = Parity(answer & 0xff);
        state->cc.cy = (answer > 0xff);
        state->cc.ac = AuxiliaryCarry(answer, state->a, ~state->d);
        state->a = answer & 0xff;
    } break;
    case 0x93: // SUB E
    {
        uint16_t answer = state->a - state->e;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.p = Parity(answer & 0xff);
        state->cc.cy = (answer > 0xff);
        state->cc.ac = AuxiliaryCarry(answer, state->a, ~state->e);
        state->a = answer & 0xff;
    } break;
    case 0x94: // SUB H
    {
        uint16_t answer = state->a - state->h;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.p = Parity(answer & 0xff);
        state->cc.cy = (answer > 0xff);
        state->cc.ac = AuxiliaryCarry(answer, state->a, ~state->h);
        state->a = answer & 0xff;
    } break;
    case 0x95: // SUB L
    {
        uint16_t answer = state->a - state->l;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.p = Parity(answer & 0xff);
        state->cc.cy = (answer > 0xff);
        state->cc.ac = AuxiliaryCarry(answer, state->a, ~state->l);
        state->a = answer & 0xff;
    } break;
    case 0x96: // SUB M
    {
        uint16_t addr = (state->h << 8) | state->l;
        uint16_t answer = state->a - state->memory[addr];
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.p = Parity(answer & 0xff);
        state->cc.cy = (answer > 0xff);
        state->cc.ac = AuxiliaryCarry(answer, state->a, ~state->memory[addr]);
        state->a = answer & 0xff;
    } break;
    case 0x97: // SUB A
               // NOTE: could be simplified
    {
        uint16_t answer = state->a - state->a;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.p = Parity(answer & 0xff);
        state->cc.cy = (answer > 0xff);
        state->cc.ac = AuxiliaryCarry(answer, state->a, ~state->a);
        state->a = answer & 0xff;
    } break;
    case 0x98: // SBB B
    {
        uint16_t answer = (uint16_t)state->a - (uint16_t)state->b - state->cc.cy;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.cy = (answer > 0xff);
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = AuxiliaryCarry(answer, state->a, ~(state->b + state->cc.cy));
        state->a = answer & 0xff;
    } break;
    case 0x99: // SBB C
    {
        uint16_t answer = (uint16_t)state->a - (uint16_t)state->c - state->cc.cy;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.cy = (answer > 0xff);
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = AuxiliaryCarry(answer, state->a, ~(state->c + state->cc.cy));
        state->a = answer & 0xff;
    } break;
    case 0x9a: // SBB D
    {
        uint16_t answer = (uint16_t)state->a - (uint16_t)state->d - state->cc.cy;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.cy = (answer > 0xff);
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = AuxiliaryCarry(answer, state->a, ~(state->d + state->cc.cy));
        state->a = answer & 0xff;
    } break;
    case 0x9b: // SBB E
    {
        uint16_t answer = (uint16_t)state->a - (uint16_t)state->e - state->cc.cy;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.cy = (answer > 0xff);
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = AuxiliaryCarry(answer, state->a, ~(state->e + state->cc.cy));
        state->a = answer & 0xff;
    } break;
    case 0x9c: // SBB H
    {
        uint16_t answer = (uint16_t)state->a - (uint16_t)state->h - state->cc.cy;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.cy = (answer > 0xff);
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = AuxiliaryCarry(answer, state->a, ~(state->h + state->cc.cy));
        state->a = answer & 0xff;
    } break;
    case 0x9d: // SBB L
    {
        uint16_t answer = (uint16_t)state->a - (uint16_t)state->l - state->cc.cy;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.cy = (answer > 0xff);
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = AuxiliaryCarry(answer, state->a, ~(state->l + state->cc.cy));
        state->a = answer & 0xff;
    } break;
    case 0x9e: // SBB M
    {
        uint16_t addr = (state->h << 8) | state->l;
        uint16_t answer = (uint16_t)state->a - (uint16_t)state->memory[addr] - state->cc.cy;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.cy = (answer > 0xff);
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = AuxiliaryCarry(answer, state->a, ~(state->memory[addr] + state->cc.cy));
        state->a = answer & 0xff;
    } break;
    case 0x9f: // SBB A
    {
        uint16_t answer = (uint16_t)state->a - (uint16_t)state->a - state->cc.cy;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.cy = (answer > 0xff);
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = AuxiliaryCarry(answer, state->a, ~(state->a + state->cc.cy));
        state->a = answer & 0xff;
    } break;
    case 0xa0: // ANA B
    {
        uint8_t answer = state->a & state->b;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.cy = 0;
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = ((state->a | state->b) & 0x08) != 0;
        state->a = answer;
    } break;
    case 0xa1: // ANA C
    {
        uint8_t answer = state->a & state->c;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.cy = 0;
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = ((state->a | state->c) & 0x08) != 0;
        state->a = answer;
    } break;
    case 0xa2: // ANA D
    {
        uint8_t answer = state->a & state->d;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.cy = 0;
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = ((state->a | state->d) & 0x08) != 0;
        state->a = answer;
    } break;
    case 0xa3: // ANA E
    {
        uint8_t answer = state->a & state->e;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.cy = 0;
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = ((state->a | state->e) & 0x08) != 0;
        state->a = answer;
    } break;
    case 0xa4: // ANA H
    {
        uint8_t answer = state->a & state->h;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.cy = 0;
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = ((state->a | state->h) & 0x08) != 0;
        state->a = answer;
    } break;
    case 0xa5: // ANA L
    {
        uint8_t answer = state->a & state->l;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.cy = 0;
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = ((state->a | state->l) & 0x08) != 0;
        state->a = answer;
    } break;
    case 0xa6: // ANA M
    {
        uint16_t addr = (state->h << 8) | state->l;
        uint8_t answer = state->a & state->memory[addr];
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.cy = 0;
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = ((state->a | state->memory[addr]) & 0x08) != 0;
        state->a = answer;
    } break;
    case 0xa7: // ANA A
    {
        uint8_t answer = state->a & state->a;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.cy = 0;
        state->cc.p = Parity(answer & 0xff);
        // TODO: understand this AuxiliaryCarry operation
        state->cc.ac = ((state->a | state->a) & 0x08) != 0;
        state->a = answer;
    } break;
    case 0xa8: // XRA B
    {
        uint8_t answer = state->a ^ state->b;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.cy = 0;
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = 0;
        state->a = answer;
    } break;
    case 0xa9: // XRA C
    {
        uint8_t answer = state->a ^ state->c;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.cy = 0;
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = 0;
        state->a = answer;
    } break;
    case 0xaa: // XRA D
    {
        uint8_t answer = state->a ^ state->d;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.cy = 0;
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = 0;
        state->a = answer;
    } break;
    case 0xab: // XRA E
    {
        uint8_t answer = state->a ^ state->e;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.cy = 0;
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = 0;
        state->a = answer;
    } break;
    case 0xac: // XRA H
    {
        uint8_t answer = state->a ^ state->h;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.cy = 0;
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = 0;
        state->a = answer;
    } break;
    case 0xad: // XRA L
    {
        uint8_t answer = state->a ^ state->l;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.cy = 0;
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = 0;
        state->a = answer;
    } break;
    case 0xae: // XRA M
    {
        uint16_t addr = (state->h << 8) | state->l;
        uint8_t answer = state->a ^ state->memory[addr];
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.cy = 0;
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = 0;
        state->a = answer;
    } break;
    case 0xaf: // XRA A
    {
        uint8_t answer = state->a ^ state->a;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.cy = 0;
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = 0;
        state->a = answer;
    } break;
    case 0xb0: // ORA B
    {
        uint8_t answer = state->a | state->b;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) != 0);
        state->cc.cy = 0;
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = 0;
        state->a = answer;

    } break;
    case 0xb1: // ORA C
    {
        uint8_t answer = state->a | state->c;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) != 0);
        state->cc.cy = 0;
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = 0;
        state->a = answer;

    } break;
    case 0xb2: // ORA D
    {
        uint8_t answer = state->a | state->d;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) != 0);
        state->cc.cy = 0;
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = 0;
        state->a = answer;

    } break;
    case 0xb3: // ORA E
    {
        uint8_t answer = state->a | state->e;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) != 0);
        state->cc.cy = 0;
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = 0;
        state->a = answer;

    } break;
    case 0xb4: // ORA H
    {
        uint8_t answer = state->a | state->h;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) != 0);
        state->cc.cy = 0;
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = 0;
        state->a = answer;

    } break;
    case 0xb5: // ORA L
    {
        uint8_t answer = state->a | state->l;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) != 0);
        state->cc.cy = 0;
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = 0;
        state->a = answer;

    } break;
    case 0xb6: // ORA M
    {
        uint16_t addr = (state->h << 8) | state->l;
        uint8_t answer = state->a | state->memory[addr];
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) != 0);
        state->cc.cy = 0;
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = 0;
        state->a = answer;

    } break;
    case 0xb7: // ORA A
    {
        uint8_t answer = state->a | state->a;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) != 0);
        state->cc.cy = 0;
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = 0;
        state->a = answer;

    } break;
    case 0xb8: // CMP B
    {
        uint16_t answer = state->a - state->b;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.cy = (answer > 0xff);
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = AuxiliaryCarry(answer, state->a, ~state->b);
    } break;
    case 0xb9: // CMP C
    {
        uint16_t answer = state->a - state->c;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.cy = (answer > 0xff);
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = AuxiliaryCarry(answer, state->a, ~state->c);
    } break;
    case 0xba: // CMP D
    {
        uint16_t answer = state->a - state->d;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.cy = (answer > 0xff);
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = AuxiliaryCarry(answer, state->a, ~state->d);
    } break;
    case 0xbb: // CMP E
    {
        uint16_t answer = state->a - state->e;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.cy = (answer > 0xff);
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = AuxiliaryCarry(answer, state->a, ~state->e);
    } break;
    case 0xbc: // CMP H
    {
        uint16_t answer = state->a - state->h;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.cy = (answer > 0xff);
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = AuxiliaryCarry(answer, state->a, ~state->h);
    } break;
    case 0xbd: // CMP L
    {
        uint16_t answer = state->a - state->l;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.cy = (answer > 0xff);
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = AuxiliaryCarry(answer, state->a, ~state->l);
    } break;
    case 0xbe: // CMP M
    {
        uint16_t addr = (state->h << 8) | state->l;
        uint16_t answer = state->a - state->memory[addr];
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.cy = (answer > 0xff);
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = AuxiliaryCarry(answer, state->a, ~state->memory[addr]);
    } break;
    case 0xbf: // CMP A
               // NOTE: could be simplified
    {
        uint16_t answer = state->a - state->a;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.cy = (answer > 0xff);
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = AuxiliaryCarry(answer, state->a, ~state->a);
    } break;
    case 0xc0: // RNZ
        if (state->cc.z == 0) {
            state->pc = (((uint16_t)(state->memory[state->sp + 1])) << 8) | (uint16_t)(state->memory[state->sp]);
            state->sp += 2;
        }
        break;
    case 0xc1: // POP B
        state->c = state->memory[state->sp];
        state->b = state->memory[state->sp + 1];
        state->sp += 2;
        break;
    case 0xc2: // JNZ addr
    {
        if (state->cc.z == 0) {
            state->pc = opcode_data(opcode);
        } else {
            state->pc += 2;
        }
    } break;
    case 0xc3: // JMP addr
    {
        state->pc = opcode_data(opcode);
    } break;
    case 0xc4: // CNZ addr
        if (state->cc.z == 0) {
            uint16_t next = state->pc + 2;
            state->memory[state->sp - 1] = ((next >> 8) & 0xff);
            state->memory[state->sp - 2] = (next & 0xff);
            state->sp -= 2;
            state->pc = opcode_data(opcode);
        } else {
            state->pc += 2;
        }
        break;
    case 0xc5: // PUSH B
        state->memory[state->sp - 1] = state->b;
        state->memory[state->sp - 2] = state->c;
        state->sp -= 2;
        break;
    case 0xc6: // ADI data
    {
        uint16_t answer = (uint16_t)state->a + (uint16_t)opcode[1];
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.cy = (answer > 0xff);
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = AuxiliaryCarry(answer, state->a, opcode[1]);
        state->a = answer & 0xff;
        state->pc += 1;
    } break;
    case 0xc7: // RST 0
        UnimplementedInstruction(state);
        break;
    case 0xc8: // RZ
        if (state->cc.z == 1) {
            state->pc = (((uint16_t)(state->memory[state->sp + 1])) << 8) | (uint16_t)(state->memory[state->sp]);
            state->sp += 2;
        }
        break;
    case 0xc9: // RET
    {
        state->pc = state->memory[state->sp] | (state->memory[state->sp + 1] << 8);
        state->sp += 2;
    } break;
    case 0xca: // JZ addr
        if (state->cc.z == 1) {
            state->pc = opcode_data(opcode);
        } else {
            state->pc += 2;
        }
        break;
    case 0xcb: // NOP
        break;
    case 0xcc: // CZ addr
        if (state->cc.z == 1) {
            uint16_t next = state->pc + 2;
            state->memory[state->sp - 1] = ((next >> 8) & 0xff);
            state->memory[state->sp - 2] = (next & 0xff);
            state->sp -= 2;
            state->pc = opcode_data(opcode);
        } else {
            state->pc += 2;
        }
        break;
    case 0xcd: // CALL addr
    {
        uint16_t next = state->pc + 2;
        state->memory[state->sp - 1] = ((next >> 8) & 0xff);
        state->memory[state->sp - 2] = (next & 0xff);
        state->sp -= 2;
        state->pc = opcode_data(opcode);
    } break;
    case 0xce: // ACI data
    {
        uint16_t answer = (uint16_t)state->a + opcode[1] + state->cc.cy;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.cy = (answer > 0xff);
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = AuxiliaryCarry(answer, state->a, opcode[1] + state->cc.cy);
        state->a = answer & 0xff;
        state->pc += 1;
    } break;
    case 0xcf: // RST 1
    {

        state->memory[state->sp - 1] = (state->pc >> 8) & 0xff;
        state->memory[state->sp - 2] = state->pc & 0xff;
        state->sp -= 2;
        state->pc = 0x08;
    } break;
    case 0xd0: // RNC
        if (state->cc.cy == 0) {
            state->pc = (((uint16_t)(state->memory[state->sp + 1])) << 8) | (uint16_t)(state->memory[state->sp]);
            state->sp += 2;
        }
        break;
    case 0xd1: // POP D
        state->e = state->memory[state->sp];
        state->d = state->memory[state->sp + 1];
        state->sp += 2;
        break;
    case 0xd2: // JNC addr
        if (state->cc.cy == 0) {
            state->pc = opcode_data(opcode);
        } else {
            state->pc += 2;
        }
        break;
    case 0xd3: // OUT data
        printf("shouldn't call OUT from the emulator\n");
        exit(1);
        break;
    case 0xd4: // CNC addr
        if (state->cc.cy == 0) {
            uint16_t next = state->pc + 2;
            state->memory[state->sp - 1] = ((next >> 8) & 0xff);
            state->memory[state->sp - 2] = (next & 0xff);
            state->sp -= 2;
            state->pc = opcode_data(opcode);
        } else {
            state->pc += 2;
        }
        break;
    case 0xd5: // PUSH D
        state->memory[state->sp - 1] = state->d;
        state->memory[state->sp - 2] = state->e;
        state->sp -= 2;
        break;
    case 0xd6: // SUI data
    {
        uint16_t answer = state->a - opcode[1];
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.p = Parity(answer & 0xff);
        state->cc.cy = (answer > 0xff);
        state->cc.ac = AuxiliaryCarry(answer, state->a, ~opcode[1]);
        state->a = answer & 0xff;
        state->pc += 1;
    } break;
    case 0xd7: // RST 2
    {
        state->memory[state->sp - 1] = (state->pc >> 8) & 0xff;
        state->memory[state->sp - 2] = state->pc & 0xff;
        state->sp -= 2;

        state->pc = 0x10;
    } break;
    case 0xd8: // RC
        if (state->cc.cy == 1) {
            state->pc = (((uint16_t)(state->memory[state->sp + 1])) << 8) | (uint16_t)(state->memory[state->sp]);
            state->sp += 2;
        }
        break;
    case 0xd9: // NOP
        break;
    case 0xda: // JC addr
        if (state->cc.cy == 1) {
            state->pc = opcode_data(opcode);
        } else {
            state->pc += 2;
        }
        break;
    case 0xdb: // IN port
        printf("shouldn't call OUT from emulator\n");
        exit(1);
        break;
    case 0xdc: // CC addr
        if (state->cc.cy == 1) {
            uint16_t next = state->pc + 2;
            state->memory[state->sp - 1] = ((next >> 8) & 0xff);
            state->memory[state->sp - 2] = (next & 0xff);
            state->sp -= 2;
            state->pc = opcode_data(opcode);
        } else {
            state->pc += 2;
        }
        break;
    case 0xdd: // NOP
        break;
    case 0xde: // SBI data
    {
        uint16_t answer = state->a - opcode[1] - state->cc.cy;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.p = Parity(answer & 0xff);
        state->cc.cy = (answer > 0xff);
        state->cc.ac = AuxiliaryCarry(answer, state->a, ~(opcode[1] + state->cc.cy));
        state->a = answer & 0xff;
        state->pc += 1;
    } break;
    case 0xdf: // RST 3
        UnimplementedInstruction(state);
        break;
    case 0xe0: // RPO
        if (state->cc.p == 0) {
            state->pc = (((uint16_t)(state->memory[state->sp + 1])) << 8) | (uint16_t)(state->memory[state->sp]);
            state->sp += 2;
        }
        break;
    case 0xe1: // POP H
        state->l = state->memory[state->sp];
        state->h = state->memory[state->sp + 1];
        state->sp += 2;
        break;
    case 0xe2: // JPO addr
        if (state->cc.p == 0) {
            state->pc = opcode_data(opcode);
        } else {
            state->pc += 2;
        }
        break;
    case 0xe3: // XTHL
    {
        uint8_t stack_top = state->memory[state->sp];
        state->memory[state->sp] = state->l;
        state->l = stack_top;
        stack_top = state->memory[state->sp + 1];
        state->memory[state->sp + 1] = state->h;
        state->h = stack_top;
    } break;
    case 0xe4: // CPO addr
        if (state->cc.p == 0) {
            uint16_t next = state->pc + 2;
            state->memory[state->sp - 1] = ((next >> 8) & 0xff);
            state->memory[state->sp - 2] = (next & 0xff);
            state->sp -= 2;
            state->pc = opcode_data(opcode);
        } else {
            state->pc += 2;
        }
        break;
    case 0xe5: // PUSH H
        state->memory[state->sp - 1] = state->h;
        state->memory[state->sp - 2] = state->l;
        state->sp -= 2;
        break;
    case 0xe6: // ANI data
    {
        uint8_t answer = state->a & opcode[1];
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.cy = 0;
        state->cc.p = Parity(answer & 0xff);
        // TODO: understand this AuxiliaryCarry operation
        state->cc.ac = ((state->a | opcode[1]) & 0x08) != 0;
        state->a = answer;
        state->pc += 1;
    } break;
    case 0xe7: // RST 4
        UnimplementedInstruction(state);
        break;
    case 0xe8: // RPE
        if (state->cc.p == 1) {
            state->pc = (((uint16_t)(state->memory[state->sp + 1])) << 8) | (uint16_t)(state->memory[state->sp]);
            state->sp += 2;
        }
        break;
    case 0xe9: // PCHL
        state->pc = (state->h << 8) | state->l;
        break;
    case 0xea: // JPE addr
        if (state->cc.p == 1) {
            state->pc = opcode_data(opcode);
        } else {
            state->pc += 2;
        }
        break;
    case 0xeb: // XCHG
    {
        uint8_t temp_h, temp_l;
        temp_h = state->h;
        temp_l = state->l;
        state->h = state->d;
        state->l = state->e;
        state->d = temp_h;
        state->e = temp_l;
    } break;
    case 0xec: // CPE addr
        if (state->cc.p == 1) {
            uint16_t next = state->pc + 2;
            state->memory[state->sp - 1] = ((next >> 8) & 0xff);
            state->memory[state->sp - 2] = (next & 0xff);
            state->sp -= 2;
            state->pc = opcode_data(opcode);
        } else {
            state->pc += 2;
        }
        break;
    case 0xed: // NOP
        break;
    case 0xee: // XRI data
    {
        uint8_t answer = state->a ^ opcode[1];
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.cy = 0;
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = 0;
        state->a = answer;
        state->pc += 1;
    } break;
    case 0xef: // RST 5
        UnimplementedInstruction(state);
        break;
    case 0xf0: // RP
        if (state->cc.s == 0) {
            state->pc = (((uint16_t)(state->memory[state->sp + 1])) << 8) | (uint16_t)(state->memory[state->sp]);
            state->sp += 2;
        }
        break;
    case 0xf1: // POP PSW
    {
        uint8_t psw = state->memory[state->sp];
        state->cc.cy = ((psw & 1) == 1);
        state->cc.p = ((psw & 4) == 4);
        state->cc.ac = ((psw & 16) == 16);
        state->cc.z = ((psw & 64) == 64);
        state->cc.s = ((psw & 128) == 128);
        state->a = state->memory[state->sp + 1];
        state->sp += 2;
    } break;
    case 0xf2: // JP addr
        if (state->cc.s == 0) {
            state->pc = opcode_data(opcode);
        } else {
            state->pc += 2;
        }
        break;
    case 0xf3: // DI
        state->int_enable = 0;
        break;
    case 0xf4: // CP addr
        if (state->cc.s == 0) {
            uint16_t next = state->pc + 2;
            state->memory[state->sp - 1] = ((next >> 8) & 0xff);
            state->memory[state->sp - 2] = (next & 0xff);
            state->sp -= 2;
            state->pc = opcode_data(opcode);
        } else {
            state->pc += 2;
        }
        break;
    case 0xf5: // PUSH PSW
    {
        state->memory[state->sp - 1] = state->a;
        /* uint8_t psw = (state->cc.cy | 1 << 1 | state->cc.p << 2 | state->cc.ac << 4 | state->cc.z << 6 | state->cc.s << 7); */
        uint8_t psw = 0;
        psw |= state->cc.s << 7;
        psw |= state->cc.z << 6;
        psw |= state->cc.ac << 4;
        psw |= state->cc.p << 2;
        psw |= 1 << 1;
        psw |= state->cc.cy << 0;
        state->memory[state->sp - 2] = psw;
        state->sp -= 2;
    } break;
    case 0xf6: // ORI data
    {
        uint8_t answer = state->a | opcode[1];
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) != 0);
        state->cc.cy = 0;
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = 0;
        state->a = answer;
        state->pc += 1;

    } break;
    case 0xf7: // RST 6
        UnimplementedInstruction(state);
        break;
    case 0xf8: // RM
        if (state->cc.s == 1) {
            state->pc = (((uint16_t)(state->memory[state->sp + 1])) << 8) | (uint16_t)(state->memory[state->sp]);
            state->sp += 2;
        }
        break;
    case 0xf9: // SPHL
        state->sp = (state->h << 8) | state->l;
        break;
    case 0xfa: // JM addr
        if (state->cc.s == 1) {
            state->pc = opcode_data(opcode);
        } else {
            state->pc += 2;
        }
        break;
    case 0xfb: // EI
        state->int_enable = 1;
        state->int_delay = 1;
        break;
    case 0xfc: // CM addr
        if (state->cc.s == 1) {
            uint16_t next = state->pc + 2;
            state->memory[state->sp - 1] = ((next >> 8) & 0xff);
            state->memory[state->sp - 2] = (next & 0xff);
            state->sp -= 2;
            state->pc = opcode_data(opcode);
        } else {
            state->pc += 2;
        }
        break;
    case 0xfd: // NOP
        break;
    case 0xfe: // CPI data
    {
        uint8_t answer = state->a - opcode[1];
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) == 0x80);
        state->cc.cy = (state->a < opcode[1]);
        state->cc.p = Parity(answer & 0xff);
        state->cc.ac = AuxiliaryCarry(answer, state->a, ~opcode[1]);
        state->pc += 1;
    } break;
    case 0xff: // DI
        state->int_enable = 0;
        break;
    }
}

void read_rom_into_memory(uint8_t *memory, char *filename, uint16_t offset) {
    FILE *f = fopen(filename, "rb");
    if (f == NULL) {
        printf("error: Couldn't open %s\n", filename);
        exit(1);
    }

    // Get the file size and read it into a memory buffer
    (void)fseek(f, 0L, SEEK_END);
    int fsize = ftell(f);
    (void)fseek(f, 0L, SEEK_SET);

    uint8_t *buffer = &memory[offset];

    size_t ret = fread(buffer, fsize, 1, f);
    if (ret != sizeof(*buffer)) {
        fprintf(stderr, "fread() failed: %zu\n", ret);
        exit(EXIT_FAILURE);
    }
    fclose(f);
}
