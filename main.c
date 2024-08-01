#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
} State8080;

State8080 *init_state_8080() {
    State8080 *state = malloc(sizeof(*state));
    memset(state, 0, sizeof(*state));
    uint8_t *memory = calloc(1 << 16, 1);
    if (memory == NULL) {
        printf("Couldn't allocate memory for 8080 state.");
        exit(1);
    }
    state->memory = memory;

    state->sp = 0xf000;

    return state;
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
        printf("NOP");
        break;
    case 0x01:
        printf("LXI B,#$%02x%02x", code[2], code[1]);
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
        printf("DAP SP");
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
    printf("\n");

    return opbytes;
}

void UnimplementedInstruction(State8080 *state) {
    printf("Error: Unimplemented instruction\n");
    exit(1);
}

uint16_t Parity(uint16_t x) {
    uint16_t sum = 0;

    for (sum = 0; x > 0; x = (x >> 1)) {
        sum += ((x & 0x0001) == 0x0001);
    }
    return (sum % 2 == 0);
}

uint16_t opcode_data(unsigned char *opcode) {
    return (opcode[2] << 8) | opcode[1];
}

/*
flags:
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) != 0);
        state->cc.cy = (answer > 0xff);
        state->cc.p = Parity(answer & 0xff);
*/

// TODO: Test the currently implemented opcodes
void Emulate8080Op(State8080 *state) {
    unsigned char *opcode = &state->memory[state->pc];

    state->pc += 1; // for the opcode

    switch (*opcode) {
    case 0x00: // NOP
        break;
    case 0x01: // LXI B, data 16
        state->c = opcode[1];
        state->b = opcode[2];
        state->pc += 2;
        break;
    case 0x02:
        UnimplementedInstruction(state);
        break;
    case 0x03:
        UnimplementedInstruction(state);
        break;
    case 0x04:
        UnimplementedInstruction(state);
        break;
    case 0x05: // DCR B
    {
        uint8_t answer = state->b - 1;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) != 0);
        state->cc.p = Parity(answer & 0xff);
        state->b = answer;
    } break;
    case 0x06: // MVI B, data
        state->b = opcode[1];
        state->pc += 1;
        break;
    case 0x07:
        UnimplementedInstruction(state);
        break;
    case 0x08:
        UnimplementedInstruction(state);
        break;
    case 0x09: // DAD B
    {
        uint32_t answer = (state->h << 8) | state->l;
        answer += (state->b << 8) | state->c;
        state->cc.cy = (answer > 0xffff);
        state->h = (answer >> 8) & 0xff;
        state->l = answer & 0xff;
    } break;
    case 0x0a:
        UnimplementedInstruction(state);
        break;
    case 0x0b:
        UnimplementedInstruction(state);
        break;
    case 0x0c:
        UnimplementedInstruction(state);
        break;
    case 0x0d: // DCR C
    {
        uint8_t answer = state->c - 1;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) != 0);
        state->cc.p = Parity(answer & 0xff);
        state->c = answer;
    } break;
    case 0x0e: // MVI C, data
        state->c = opcode[1];
        state->pc += 1;
        break;
    case 0x0f: // RRC
    {
        uint8_t a0 = state->a & 0x01;
        state->cc.cy = a0;
        state->a = (state->a >> 1) | (a0 << 7);
    } break;
    case 0x10:
        UnimplementedInstruction(state);
        break;
    case 0x11: // LXI D, data 16
        state->d = opcode[2];
        state->e = opcode[1];
        state->pc += 2;
        break;
    case 0x12:
        UnimplementedInstruction(state);
        break;
    case 0x13: // INX D
               // NOTE: Do nothing on overflow
    {
        uint32_t answer = ((state->d << 8) | state->e) + 1;
        state->d = (answer >> 8) & 0xff;
        state->e = answer & 0xff;
    } break;
    case 0x14:
        UnimplementedInstruction(state);
        break;
    case 0x15:
        UnimplementedInstruction(state);
        break;
    case 0x16:
        UnimplementedInstruction(state);
        break;
    case 0x17:
        UnimplementedInstruction(state);
        break;
    case 0x18:
        UnimplementedInstruction(state);
        break;
    case 0x19: // DAD D
    {
        uint32_t answer = (state->h << 8) | state->l;
        answer += (state->d << 8) | state->e;
        state->cc.cy = (answer > 0xffff);
        state->h = (answer >> 8) & 0xff;
        state->l = answer & 0xff;
    } break;
    case 0x1a: // LDAX D
    {
        uint16_t addr = (state->d << 8) | state->e;
        state->a = state->memory[addr];
    } break;
    case 0x1b:
        UnimplementedInstruction(state);
        break;
    case 0x1c:
        UnimplementedInstruction(state);
        break;
    case 0x1d:
        UnimplementedInstruction(state);
        break;
    case 0x1e:
        UnimplementedInstruction(state);
        break;
    case 0x1f:
        UnimplementedInstruction(state);
        break;
    case 0x20:
        UnimplementedInstruction(state);
        break;
    case 0x21: // LXI H, data 16
        state->h = opcode[2];
        state->l = opcode[1];
        state->pc += 2;
        break;
    case 0x22:
        UnimplementedInstruction(state);
        break;
    case 0x23: // INX H
               // NOTE: Do nothing on overflow
    {
        uint32_t answer = ((state->h << 8) | state->l) + 1;
        state->h = (answer >> 8) & 0xff;
        state->l = answer & 0xff;
    } break;
    case 0x24:
        UnimplementedInstruction(state);
        break;
    case 0x25:
        UnimplementedInstruction(state);
        break;
    case 0x26: // MVI H, data
        state->h = opcode[1];
        state->pc += 1;
        break;
    case 0x27:
        UnimplementedInstruction(state);
        break;
    case 0x28:
        UnimplementedInstruction(state);
        break;
    case 0x29: // DAD H
    {
        uint32_t answer = (state->h << 8) | state->l;
        answer += answer;
        state->cc.cy = (answer > 0xffff);
        state->h = (answer >> 8) & 0xff;
        state->l = answer & 0xff;
    } break;
    case 0x2a:
        UnimplementedInstruction(state);
        break;
    case 0x2b:
        UnimplementedInstruction(state);
        break;
    case 0x2c:
        UnimplementedInstruction(state);
        break;
    case 0x2d:
        UnimplementedInstruction(state);
        break;
    case 0x2e:
        UnimplementedInstruction(state);
        break;
    case 0x2f:
        UnimplementedInstruction(state);
        break;
    case 0x30:
        UnimplementedInstruction(state);
        break;
    case 0x31: // LXI SP, data 16
        state->sp = opcode_data(opcode);
        state->pc += 2;
        break;
    case 0x32: // STA Addr
               // WARN: Not tested
    {
        uint16_t addr = (opcode[2] << 8) | opcode[1];
        state->memory[addr] = state->a;
        state->pc += 2;
    } break;
    case 0x33:
        UnimplementedInstruction(state);
        break;
    case 0x34:
        UnimplementedInstruction(state);
        break;
    case 0x35:
        UnimplementedInstruction(state);
        break;
    case 0x36: // MVI M, data
    {
        uint16_t addr = (state->h << 8) | state->l;
        state->memory[addr] = opcode[1];
        state->pc += 1;
    } break;
    case 0x37:
        UnimplementedInstruction(state);
        break;
    case 0x38:
        UnimplementedInstruction(state);
        break;
    case 0x39:
        UnimplementedInstruction(state);
        break;
    case 0x3a: // LDA data 16
    {
        uint16_t addr = (opcode[2] << 8) | opcode[1];
        state->a = state->memory[addr];
        state->pc += 2;
    } break;
    case 0x3b:
        UnimplementedInstruction(state);
        break;
    case 0x3c:
        UnimplementedInstruction(state);
        break;
    case 0x3d: // DCR A
        UnimplementedInstruction(state);
        break;
    case 0x3e: // MVI A, data
        state->a = opcode[1];
        state->pc += 1;
        break;
    case 0x3f:
        UnimplementedInstruction(state);
        break;
    case 0x40: // MOV B,B
        UnimplementedInstruction(state);
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
    case 0x44:
        UnimplementedInstruction(state);
        break;
    case 0x45:
        UnimplementedInstruction(state);
        break;
    case 0x46:
        UnimplementedInstruction(state);
        break;
    case 0x47:
        UnimplementedInstruction(state);
        break;
    case 0x48:
        UnimplementedInstruction(state);
        break;
    case 0x49:
        UnimplementedInstruction(state);
        break;
    case 0x4a:
        UnimplementedInstruction(state);
        break;
    case 0x4b:
        UnimplementedInstruction(state);
        break;
    case 0x4c:
        UnimplementedInstruction(state);
        break;
    case 0x4d:
        UnimplementedInstruction(state);
        break;
    case 0x4e:
        UnimplementedInstruction(state);
        break;
    case 0x4f:
        UnimplementedInstruction(state);
        break;
    case 0x50:
        UnimplementedInstruction(state);
        break;
    case 0x51:
        UnimplementedInstruction(state);
        break;
    case 0x52:
        UnimplementedInstruction(state);
        break;
    case 0x53:
        UnimplementedInstruction(state);
        break;
    case 0x54:
        UnimplementedInstruction(state);
        break;
    case 0x55:
        UnimplementedInstruction(state);
        break;
    case 0x56: // MOV D,M
    {
        uint16_t addr = (state->h << 8) | state->l;
        state->d = state->memory[addr];
    } break;
    case 0x57:
        UnimplementedInstruction(state);
        break;
    case 0x58:
        UnimplementedInstruction(state);
        break;
    case 0x59:
        UnimplementedInstruction(state);
        break;
    case 0x5a:
        UnimplementedInstruction(state);
        break;
    case 0x5b:
        UnimplementedInstruction(state);
        break;
    case 0x5c:
        UnimplementedInstruction(state);
        break;
    case 0x5d:
        UnimplementedInstruction(state);
        break;
    case 0x5e: // MOV E,M
    {
        uint16_t addr = (state->h << 8) | state->l;
        state->e = state->memory[addr];
    } break;
    case 0x5f:
        UnimplementedInstruction(state);
        break;
    case 0x60:
        UnimplementedInstruction(state);
        break;
    case 0x61:
        UnimplementedInstruction(state);
        break;
    case 0x62:
        UnimplementedInstruction(state);
        break;
    case 0x63:
        UnimplementedInstruction(state);
        break;
    case 0x64:
        UnimplementedInstruction(state);
        break;
    case 0x65:
        UnimplementedInstruction(state);
        break;
    case 0x66: // MOV H,M
    {
        uint16_t addr = (state->h << 8) | state->l;
        state->h = state->memory[addr];
    } break;
    case 0x67:
        UnimplementedInstruction(state);
        break;
    case 0x68:
        UnimplementedInstruction(state);
        break;
    case 0x69:
        UnimplementedInstruction(state);
        break;
    case 0x6a:
        UnimplementedInstruction(state);
        break;
    case 0x6b:
        UnimplementedInstruction(state);
        break;
    case 0x6c:
        UnimplementedInstruction(state);
        break;
    case 0x6d:
        UnimplementedInstruction(state);
        break;
    case 0x6e:
        UnimplementedInstruction(state);
        break;
    case 0x6f: // MOV L,A
        state->l = state->a;
        break;
    case 0x70:
        UnimplementedInstruction(state);
        break;
    case 0x71:
        UnimplementedInstruction(state);
        break;
    case 0x72:
        UnimplementedInstruction(state);
        break;
    case 0x73:
        UnimplementedInstruction(state);
        break;
    case 0x74:
        UnimplementedInstruction(state);
        break;
    case 0x75:
        UnimplementedInstruction(state);
        break;
    case 0x76:
        UnimplementedInstruction(state);
        break;
    case 0x77: // MOV M,A
               // WARN: Not tested
    {
        uint16_t addr = (state->h << 8) | state->l;
        state->memory[addr] = state->a;
    } break;
    case 0x78:
        UnimplementedInstruction(state);
        break;
    case 0x79:
        UnimplementedInstruction(state);
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
    case 0x7d:
        UnimplementedInstruction(state);
        break;
    case 0x7e: // MOV A,M
    {
        uint16_t addr = (state->h << 8) | state->l;
        state->a = state->memory[addr];
    } break;
    case 0x7f:
        UnimplementedInstruction(state);
        break;
    case 0x80: // ADD B
    {
        uint16_t answer = (uint16_t)state->a + (uint16_t)state->b;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) != 0);
        state->cc.cy = (answer > 0xff);
        state->cc.p = Parity(answer & 0xff);
        state->a = answer & 0xff;
    } break;
    case 0x81:
        UnimplementedInstruction(state);
        break;
    case 0x82:
        UnimplementedInstruction(state);
        break;
    case 0x83:
        UnimplementedInstruction(state);
        break;
    case 0x84:
        UnimplementedInstruction(state);
        break;
    case 0x85:
        UnimplementedInstruction(state);
        break;
    case 0x86: // ADD M
    {
        uint16_t offset = (state->h << 8) | (state->l);
        uint16_t answer = (uint16_t)state->a + state->memory[offset];
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) != 0);
        state->cc.cy = (answer > 0xff);
        state->cc.p = Parity(answer & 0xff);
        state->a = answer & 0xff;
    } break;
    case 0x87:
        UnimplementedInstruction(state);
        break;
    case 0x88:
        UnimplementedInstruction(state);
        break;
    case 0x89:
        UnimplementedInstruction(state);
        break;
    case 0x8a:
        UnimplementedInstruction(state);
        break;
    case 0x8b:
        UnimplementedInstruction(state);
        break;
    case 0x8c:
        UnimplementedInstruction(state);
        break;
    case 0x8d:
        UnimplementedInstruction(state);
        break;
    case 0x8e:
        UnimplementedInstruction(state);
        break;
    case 0x8f:
        UnimplementedInstruction(state);
        break;
    case 0x90:
        UnimplementedInstruction(state);
        break;
    case 0x91:
        UnimplementedInstruction(state);
        break;
    case 0x92:
        UnimplementedInstruction(state);
        break;
    case 0x93:
        UnimplementedInstruction(state);
        break;
    case 0x94:
        UnimplementedInstruction(state);
        break;
    case 0x95:
        UnimplementedInstruction(state);
        break;
    case 0x96:
        UnimplementedInstruction(state);
        break;
    case 0x97:
        UnimplementedInstruction(state);
        break;
    case 0x98:
        UnimplementedInstruction(state);
        break;
    case 0x99:
        UnimplementedInstruction(state);
        break;
    case 0x9a:
        UnimplementedInstruction(state);
        break;
    case 0x9b:
        UnimplementedInstruction(state);
        break;
    case 0x9c:
        UnimplementedInstruction(state);
        break;
    case 0x9d:
        UnimplementedInstruction(state);
        break;
    case 0x9e:
        UnimplementedInstruction(state);
        break;
    case 0x9f:
        UnimplementedInstruction(state);
        break;
    case 0xa0:
        UnimplementedInstruction(state);
        break;
    case 0xa1:
        UnimplementedInstruction(state);
        break;
    case 0xa2:
        UnimplementedInstruction(state);
        break;
    case 0xa3:
        UnimplementedInstruction(state);
        break;
    case 0xa4:
        UnimplementedInstruction(state);
        break;
    case 0xa5:
        UnimplementedInstruction(state);
        break;
    case 0xa6:
        UnimplementedInstruction(state);
        break;
    case 0xa7: // ANA A
    {
        uint8_t answer = state->a & state->a;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) != 0);
        state->cc.cy = 0;
        state->cc.p = Parity(answer & 0xff);
        state->a = answer;
    } break;
    case 0xa8:
        UnimplementedInstruction(state);
        break;
    case 0xa9:
        UnimplementedInstruction(state);
        break;
    case 0xaa:
        UnimplementedInstruction(state);
        break;
    case 0xab:
        UnimplementedInstruction(state);
        break;
    case 0xac:
        UnimplementedInstruction(state);
        break;
    case 0xad:
        UnimplementedInstruction(state);
        break;
    case 0xae:
        UnimplementedInstruction(state);
        break;
    case 0xaf: // XRA A
    {
        uint8_t answer = state->a ^ state->a;
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) != 0);
        state->cc.cy = 0;
        state->cc.p = Parity(answer & 0xff);
        state->a = answer;
    } break;
    case 0xb0:
        UnimplementedInstruction(state);
        break;
    case 0xb1:
        UnimplementedInstruction(state);
        break;
    case 0xb2:
        UnimplementedInstruction(state);
        break;
    case 0xb3:
        UnimplementedInstruction(state);
        break;
    case 0xb4:
        UnimplementedInstruction(state);
        break;
    case 0xb5:
        UnimplementedInstruction(state);
        break;
    case 0xb6:
        UnimplementedInstruction(state);
        break;
    case 0xb7:
        UnimplementedInstruction(state);
        break;
    case 0xb8:
        UnimplementedInstruction(state);
        break;
    case 0xb9:
        UnimplementedInstruction(state);
        break;
    case 0xba:
        UnimplementedInstruction(state);
        break;
    case 0xbb:
        UnimplementedInstruction(state);
        break;
    case 0xbc:
        UnimplementedInstruction(state);
        break;
    case 0xbd:
        UnimplementedInstruction(state);
        break;
    case 0xbe:
        UnimplementedInstruction(state);
        break;
    case 0xbf:
        UnimplementedInstruction(state);
        break;
    case 0xc0:
        UnimplementedInstruction(state);
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
    case 0xc4:
        UnimplementedInstruction(state);
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
        state->cc.s = ((answer & 0x80) != 0);
        state->cc.cy = (answer > 0xff);
        state->cc.p = Parity(answer & 0xff);
        state->a = answer & 0xff;
        state->pc += 1;
    } break;
    case 0xc7:
        UnimplementedInstruction(state);
        break;
    case 0xc8:
        UnimplementedInstruction(state);
        break;
    case 0xc9: // RET
    {
        state->pc = (((uint16_t)(state->memory[state->sp + 1])) << 8) | (uint16_t)(state->memory[state->sp]);
        state->sp += 2;
    } break;
    case 0xca:
        UnimplementedInstruction(state);
        break;
    case 0xcb:
        UnimplementedInstruction(state);
        break;
    case 0xcc:
        UnimplementedInstruction(state);
        break;
    case 0xcd: // CALL addr
    {
        uint16_t next = state->pc + 2;
        state->memory[state->sp - 1] = ((next >> 8) & 0xff);
        state->memory[state->sp - 2] = (next & 0xff);
        state->sp -= 2;
        state->pc = opcode_data(opcode);
    } break;
    case 0xce:
        UnimplementedInstruction(state);
        break;
    case 0xcf:
        UnimplementedInstruction(state);
        break;
    case 0xd0:
        UnimplementedInstruction(state);
        break;
    case 0xd1: // POP D
        state->e = state->memory[state->sp];
        state->d = state->memory[state->sp + 1];
        state->sp += 2;
        break;
    case 0xd2:
        UnimplementedInstruction(state);
        break;
    case 0xd3: // OUT data
               // TODO: Revisit after implementing data bus
        state->pc += 1;
        break;
    case 0xd4:
        UnimplementedInstruction(state);
        break;
    case 0xd5: // PUSH D
        state->memory[state->sp - 1] = state->d;
        state->memory[state->sp - 2] = state->e;
        state->sp -= 2;
        break;
    case 0xd6:
        UnimplementedInstruction(state);
        break;
    case 0xd7:
        UnimplementedInstruction(state);
        break;
    case 0xd8:
        UnimplementedInstruction(state);
        break;
    case 0xd9:
        UnimplementedInstruction(state);
        break;
    case 0xda:
        UnimplementedInstruction(state);
        break;
    case 0xdb:
        UnimplementedInstruction(state);
        break;
    case 0xdc:
        UnimplementedInstruction(state);
        break;
    case 0xdd:
        UnimplementedInstruction(state);
        break;
    case 0xde:
        UnimplementedInstruction(state);
        break;
    case 0xdf:
        UnimplementedInstruction(state);
        break;
    case 0xe0:
        UnimplementedInstruction(state);
        break;
    case 0xe1: // POP H
        state->l = state->memory[state->sp];
        state->h = state->memory[state->sp + 1];
        state->sp += 2;
        break;
    case 0xe2:
        UnimplementedInstruction(state);
        break;
    case 0xe3:
        UnimplementedInstruction(state);
        break;
    case 0xe4:
        UnimplementedInstruction(state);
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
        state->cc.s = ((answer & 0x80) != 0);
        state->cc.cy = 0;
        state->cc.p = Parity(answer & 0xff);
        state->a = answer;
        state->pc += 1;
    } break;
    case 0xe7:
        UnimplementedInstruction(state);
        break;
    case 0xe8:
        UnimplementedInstruction(state);
        break;
    case 0xe9:
        UnimplementedInstruction(state);
        break;
    case 0xea:
        UnimplementedInstruction(state);
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
    case 0xec:
        UnimplementedInstruction(state);
        break;
    case 0xed:
        UnimplementedInstruction(state);
        break;
    case 0xee:
        UnimplementedInstruction(state);
        break;
    case 0xef:
        UnimplementedInstruction(state);
        break;
    case 0xf0:
        UnimplementedInstruction(state);
        break;
    case 0xf1: // POP PSW
               // WARN: Not tested
    {
        /* UnimplementedInstruction(state); */
        uint8_t psw = state->memory[state->sp];
        state->cc.cy = ((psw & 1) == 1);
        state->cc.p = ((psw & 4) == 4);
        state->cc.ac = ((psw & 16) == 16);
        state->cc.z = ((psw & 64) == 64);
        state->cc.s = ((psw & 128) == 128);
        state->a = state->memory[state->sp + 1];
        state->sp += 2;
    } break;
    case 0xf2:
        UnimplementedInstruction(state);
        break;
    case 0xf3:
        UnimplementedInstruction(state);
        break;
    case 0xf4:
        UnimplementedInstruction(state);
        break;
    case 0xf5: // PUSH PSW
               // WARN: Not tested
               // WARN: Not the same as tutorial101
    {
        state->memory[state->sp - 1] = state->a;
        uint8_t psw = (state->cc.cy | 1 << 1 | state->cc.p << 2 | state->cc.ac << 4 | state->cc.z << 6 | state->cc.s << 7);
        state->memory[state->sp - 2] = psw;
        state->sp -= 2;
    } break;
    case 0xf6:
        UnimplementedInstruction(state);
        break;
    case 0xf7:
        UnimplementedInstruction(state);
        break;
    case 0xf8:
        UnimplementedInstruction(state);
        break;
    case 0xf9:
        UnimplementedInstruction(state);
        break;
    case 0xfa:
        UnimplementedInstruction(state);
        break;
    case 0xfb: // EI
        state->int_enable = 1;
        break;
    case 0xfc:
        UnimplementedInstruction(state);
        break;
    case 0xfd:
        UnimplementedInstruction(state);
        break;
    case 0xfe: // CPI data
    {
        uint16_t answer = state->a - opcode[1];
        state->cc.z = ((answer & 0xff) == 0);
        state->cc.s = ((answer & 0x80) != 0);
        state->cc.cy = (answer > 0xff);
        state->cc.p = Parity(answer & 0xff);
        state->pc += 1;
    } break;
    case 0xff:
        UnimplementedInstruction(state);
        break;
    }
}

void print_state(State8080 *state) {
    printf("\tCY=%d, P=%d, S=%d, Z=%d, I=%d\n", state->cc.cy, state->cc.p, state->cc.s, state->cc.z, state->int_enable);
    printf("\tA=%02x, BC=%02x%02x, DE=%02x%02x, HL=%02x%02x\n", state->a, state->b, state->c, state->d, state->e, state->h, state->l);
    printf("\tPC=%04x, SP=%04x\n", state->pc, state->sp);
}

void read_rom_into_memory(State8080 *state, char *filename, uint16_t offset) {
    FILE *f = fopen(filename, "rb");
    if (f == NULL) {
        printf("error: Couldn't open %s\n", filename);
        exit(1);
    }

    // Get the file size and read it into a memory buffer
    (void)fseek(f, 0L, SEEK_END);
    int fsize = ftell(f);
    (void)fseek(f, 0L, SEEK_SET);

    uint8_t *buffer = &state->memory[offset];

    (void)fread(buffer, fsize, 1, f);
    fclose(f);
}

int main() {

    State8080 *state = init_state_8080();
    read_rom_into_memory(state, "space-invaders.rom", 0x0000);
    print_state(state);

    uint32_t max_iter = 100000;
    /* uint32_t max_iter = 40147; they agree */
    /* uint32_t max_iter = 40205; */
    uint32_t n = 0;

    while (n++ < max_iter) {
        printf("n=%d\n", n);
        (void)Disassemble8080Op(state->memory, state->pc);
        Emulate8080Op(state);
        print_state(state);
    }

    return 0;
}
