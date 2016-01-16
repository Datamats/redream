#include "jit/frontend/sh4/sh4_frontend.h"

using namespace dvm::hw;
using namespace dvm::jit;
using namespace dvm::jit::frontend::sh4;
using namespace dvm::jit::ir;

SH4Frontend::SH4Frontend(Memory &memory) : Frontend(memory) {}

std::unique_ptr<IRBuilder> SH4Frontend::BuildBlock(uint32_t addr,
                                                   const void *guest_ctx) {
  std::unique_ptr<SH4Builder> builder(new SH4Builder(memory_));

  builder->Emit(addr, *reinterpret_cast<const SH4Context *>(guest_ctx));

  return std::unique_ptr<IRBuilder>(std::move(builder));
}