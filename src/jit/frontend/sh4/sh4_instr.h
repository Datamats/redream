#ifndef SH4_INSTR_H
#define SH4_INSTR_H

#include "jit/ir/ir_builder.h"

namespace dvm {
namespace jit {
namespace frontend {
namespace sh4 {

enum Opcode {
#define SH4_INSTR(name, desc, instr_code, cycles, flags) OP_##name,
#include "jit/frontend/sh4/sh4_instr.inc"
#undef SH4_INSTR
  NUM_OPCODES
};

enum OpcodeFlag {
  OP_FLAG_BRANCH = 0x1,
  OP_FLAG_CONDITIONAL = 0x2,
  OP_FLAG_DELAYED = 0x4,
  OP_FLAG_SET_T = 0x8,
  OP_FLAG_SET_FPSCR = 0x10
};

struct InstrType {
  static void GetParamMask(const char *instr_code, char c, uint16_t *mask,
                           uint16_t *shift) {
    size_t len = strlen(instr_code);
    if (mask) *mask = 0;
    if (shift) *shift = 0;
    for (size_t i = 0; i < len; i++) {
      if ((!c && instr_code[i] == '1') || (c && instr_code[i] == c)) {
        if (mask) *mask |= (1 << (len - i - 1));
        if (shift) *shift = static_cast<uint16_t>(len - i - 1);
      }
    }
  }

  InstrType(Opcode op, const char *name, const char *desc,
            const char *instr_code, int cycles, unsigned flags)
      : op(op), name(name), desc(desc), cycles(cycles), flags(flags) {
    GetParamMask(instr_code, 0, &opcode_mask, nullptr);
    GetParamMask(instr_code, 'i', &imm_mask, &imm_shift);
    GetParamMask(instr_code, 'd', &disp_mask, &disp_shift);
    GetParamMask(instr_code, 'm', &Rm_mask, &Rm_shift);
    GetParamMask(instr_code, 'n', &Rn_mask, &Rn_shift);
    param_mask = imm_mask | disp_mask | Rm_mask | Rn_mask;
  }

  Opcode op;
  const char *name;
  const char *desc;
  uint16_t opcode_mask;
  uint16_t imm_mask, imm_shift;
  uint16_t disp_mask, disp_shift;
  uint16_t Rm_mask, Rm_shift;
  uint16_t Rn_mask, Rn_shift;
  uint16_t param_mask;
  int cycles;
  unsigned flags;
};

struct Instr {
  uint32_t addr;
  uint16_t opcode;
  InstrType *type;
  uint16_t Rm;
  uint16_t Rn;
  uint16_t disp;
  uint16_t imm;
};

extern InstrType instrs[NUM_OPCODES];
extern InstrType *instr_lookup[UINT16_MAX];

bool Disasm(Instr *i);
void Dump(Instr *i);
}
}
}
}

#endif