#ifndef SH4_CODE_CACHE_H
#define SH4_CODE_CACHE_H

#include "jit/backend/x64/x64_backend.h"
#include "jit/frontend/sh4/sh4_frontend.h"
#include "jit/ir/passes/pass_runner.h"
#include "sys/exception_handler.h"

namespace re {
namespace hw {

class Memory;

// executable code sits between 0x0c000000 and 0x0d000000 (16mb). each instr
// is 2 bytes, making for a maximum of 0x1000000 >> 1 blocks
#define BLOCK_ADDR_SHIFT 1
#define BLOCK_ADDR_MASK (~0xfc000000)
#define BLOCK_OFFSET(addr) ((addr & BLOCK_ADDR_MASK) >> BLOCK_ADDR_SHIFT)
#define MAX_BLOCKS (0x1000000 >> BLOCK_ADDR_SHIFT)

struct SH4BlockEntry;

typedef std::map<uint32_t, SH4BlockEntry *> BlockMap;
typedef std::map<uintptr_t, SH4BlockEntry *> ReverseBlockMap;

struct SH4BlockEntry {
  jit::backend::BlockPointer run;
  int flags;
  BlockMap::iterator it;
  ReverseBlockMap::iterator rit;
};

class SH4CodeCache {
 public:
  SH4CodeCache(hw::Memory *memory, jit::backend::BlockPointer default_block);
  ~SH4CodeCache();

  // originally, GetBlock looked something like this:
  //
  // RuntimeBlock *GetBlock(uint32_t addr) {
  //   RuntimeBlock *block = blocks_[BLOCK_OFFSET(addr)];
  //   if (!block) { ... compile block ... }
  //   return block;
  // }
  //
  // however, the conditional to check for a block's existence performs very
  // poorly when called millions of times per second, and the most common case
  // is that the block already exists in the cache.
  //
  // to work around this, GetBlock has been changed to always return a valid
  // block, and the cache is initialized with all entries pointing to a special
  // default block. this default block, when called, will compile the actual
  // block and update the cache to point to it
  SH4BlockEntry *GetBlock(uint32_t addr) {
    int offset = BLOCK_OFFSET(addr);
    CHECK_LT(offset, MAX_BLOCKS);
    return &blocks_[offset];
  }
  SH4BlockEntry *CompileBlock(uint32_t addr, int max_instrs, void *guest_ctx);
  void InvalidateBlocks(uint32_t addr);
  void ResetBlocks();

 private:
  static bool HandleException(void *ctx, sys::Exception &ex);
  SH4BlockEntry *LookupBlock(uint32_t guest_addr);
  SH4BlockEntry *LookupBlockReverse(uintptr_t host_addr);

  sys::ExceptionHandlerHandle eh_handle_;
  jit::frontend::Frontend *frontend_;
  jit::backend::Backend *backend_;
  jit::ir::passes::PassRunner pass_runner_;

  jit::backend::BlockPointer default_block_;
  SH4BlockEntry *blocks_;
  BlockMap block_map_;
  ReverseBlockMap reverse_block_map_;
};
}
}

#endif
