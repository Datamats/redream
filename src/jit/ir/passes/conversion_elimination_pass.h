#ifndef CONVERSION_ELIMINATION_PASS_H
#define CONVERSION_ELIMINATION_PASS_H

#include "jit/backend/backend.h"
#include "jit/ir/passes/pass_runner.h"

namespace re {
namespace jit {
namespace ir {
namespace passes {

class ConversionEliminationPass : public Pass {
 public:
  const char *name() { return "cve"; }

  void Run(IRBuilder &builder);
};
}
}
}
}

#endif