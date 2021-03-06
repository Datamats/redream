// Copyright 2014, VIXL authors
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//   * Redistributions of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//   * Redistributions in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//   * Neither the name of ARM Limited nor the names of its contributors may be
//     used to endorse or promote products derived from this software without
//     specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifdef VIXL_INCLUDE_SIMULATOR_AARCH64

#ifndef VIXL_AARCH64_DEBUGGER_AARCH64_H_
#define VIXL_AARCH64_DEBUGGER_AARCH64_H_

#include <cctype>
#include <cerrno>
#include <climits>
#include <vector>

#include "../globals-vixl.h"
#include "../utils-vixl.h"

#include "constants-aarch64.h"
#include "simulator-aarch64.h"

namespace vixl {
namespace aarch64 {

// Forward declarations.
class DebugCommand;
class Token;
class FormatToken;

class Debugger : public Simulator {
 public:
  explicit Debugger(Decoder* decoder, FILE* stream = stdout);
  ~Debugger();

  virtual void Run() VIXL_OVERRIDE;
  virtual void VisitException(const Instruction* instr) VIXL_OVERRIDE;

  bool IsDebuggerActive() const { return debugger_active_; }
  void ActivateDebugger() { debugger_active_ = true; }
  void DeactivateDebugger() { debugger_active_ = false; }

  // Numbers of instructions to execute before the debugger shell is given
  // back control.
  int64_t GetSteps() const { return steps_; }
  VIXL_DEPRECATED("GetSteps", int64_t steps() const) { return GetSteps(); }

  void SetSteps(int64_t value) {
    VIXL_ASSERT(value >= 0);
    steps_ = value;
  }
  VIXL_DEPRECATED("SetSteps", void set_steps(int64_t value)) {
    return SetSteps(value);
  }

  void PrintInstructions(const void* address,
                         int64_t count = 1,
                         const char* prefix = "");
  void PrintNextInstruction() { PrintInstructions(ReadPc(), 1, "Next: "); }
  void PrintMemory(const uint8_t* address,
                   const FormatToken* format,
                   int64_t count = 1);
  void PrintRegister(const Register& target_reg,
                     const char* name,
                     const FormatToken* format);
  void PrintFPRegister(const FPRegister& target_fpreg,
                       const FormatToken* format);

 private:
  char* ReadCommandLine(const char* prompt, char* buffer, int length);
  void RunDebuggerShell();
  void DoBreakpoint(const Instruction* instr);

  bool debugger_active_;
  int64_t steps_;
  DebugCommand* last_command_;
  PrintDisassembler* disasm_;
  Decoder* printer_;

  // Length of the biggest command line accepted by the debugger shell.
  static const int kMaxDebugShellLine = 256;
};

}  // namespace aarch64
}  // namespace vixl

#endif  // VIXL_AARCH64_DEBUGGER_AARCH64_H_

#endif  // VIXL_INCLUDE_SIMULATOR_AARCH64
