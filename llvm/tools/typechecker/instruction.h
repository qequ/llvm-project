#ifndef INSTRUCTION
#define INSTRUCTION_H
#include <list>
#include <string>
#include <variant>


using Mnemonic = std::string;

enum class X86Registers {
  rdi,
  rsi,
  ax,
  bx,
  cx,
  dx,
  bp,
  sp
  // add more later
};

using ImmediateInteger = uint64_t;

struct Address {};

struct NoPointer {};
struct Pointer {
  unsigned int indirections;
};
using TypeInfo = std::variant<NoPointer, Pointer>;

struct Operand {
  std::variant<X86Registers, ImmediateInteger, Address> operandData;
  TypeInfo typeData; // to be calculated
};


#endif