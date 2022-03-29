#ifndef LLVM_INSTRUCTION
#define LLVM_INSTRUCTION_H
#include <list>
#include <string>
#include <variant>
#include <optional>
#pragma once
namespace llvm {

using Mnemonic = std::string;

enum class X86Registers {
  rdi,
  rsi,
  
  ax,
  bx,
  cx,
  dx,
  
  al,
  bl,
  cl,
  dl,

  rbp,
  rsp,
  
  eax,
  ebx,
  ecx,
  edx,
  
  rax,
  rbx,
  rcx,
  rdx,
  
  ah,
  bh,
  ch,
  dh

};

using ImmediateInteger = uint64_t;

struct Address {};

struct Types {
    struct NoPointer {};
    struct Pointer {
    unsigned int indirections;
    };

};

using TypeData = std::variant<Types::NoPointer, Types::Pointer>;

struct Operand {
  std::variant<X86Registers, ImmediateInteger, Address> operandData;
  std::optional<TypeData> typeData; // to be calculated
};

struct Instruction {
  Mnemonic mnemonic;
  std::list<Operand> operands;
};
} // namespace llvm
#endif