//===- RTGTestOps.cpp - Implement the RTG operations ----------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements the RTGTest ops.
//
//===----------------------------------------------------------------------===//

#include "circt/Dialect/RTGTest/IR/RTGTestOps.h"
#include "mlir/IR/Builders.h"
#include "llvm/ADT/APInt.h"

using namespace circt;
using namespace rtgtest;

//===----------------------------------------------------------------------===//
// TestInstrAOp
//===----------------------------------------------------------------------===//

APInt TestInstrAOp::getBinary(ArrayRef<APInt> operands) {
  return APInt(7, 0b1110001).concat(operands[0]).concat(operands[1]);
}

//===----------------------------------------------------------------------===//
// TestInstrBOp
//===----------------------------------------------------------------------===//

APInt TestInstrBOp::getBinary(ArrayRef<APInt> operands) {
  return APInt(7, 0b1110000)
      .concat(operands[0])
      .concat(operands[1])
      .concat(operands[2])
      .concat(APInt(9, 0));
}

//===----------------------------------------------------------------------===//
// IntegerRegisterOp
//===----------------------------------------------------------------------===//
unsigned IntegerRegisterOp::getClassIndex() { return getNumber(); }
APInt IntegerRegisterOp::getClassIndexBinary() { return APInt(2, getClassIndex()); }

std::string IntegerRegisterOp::getRegisterAssembly() {
  return "i" + std::to_string(getClassIndex());
}

llvm::BitVector IntegerRegisterOp::getAllowedRegs() {
  llvm::BitVector allowed(static_cast<unsigned>(RTGTestRegisters::MAX));
  if (getNumber() < 0)
    return allowed.set(0,4);
  return allowed.set(getNumber());
}

unsigned IntegerRegisterOp::getFixedReg() {
  if (getNumber() < 0)
    return ~0U;
  return getNumber();
}

void IntegerRegisterOp::setFixedReg(unsigned reg) {
  setNumber(reg);
}

LogicalResult IntegerRegisterOp::verify() {
  if (getNumber() > static_cast<unsigned>(RTGTestRegisters::i_3))
    return emitOpError("'number' must be smaller than 4");

  return success();
}

//===----------------------------------------------------------------------===//
// FloatRegisterOp
//===----------------------------------------------------------------------===//

unsigned FloatRegisterOp::getClassIndex() { return getNumber() - static_cast<unsigned>(RTGTestRegisters::f_0); }
APInt FloatRegisterOp::getClassIndexBinary() { return APInt(2, getClassIndex()); }

std::string FloatRegisterOp::getRegisterAssembly() {
  return "f" + std::to_string(getClassIndex());
}

llvm::BitVector FloatRegisterOp::getAllowedRegs() {
  llvm::BitVector allowed(static_cast<unsigned>(RTGTestRegisters::MAX));
  if (getNumber() < 0)
    return allowed.set(4,10);
  return allowed.set(getNumber());
}

unsigned FloatRegisterOp::getFixedReg() {
  if (getNumber() < 0)
    return ~0U;
  return getNumber();
}

void FloatRegisterOp::setFixedReg(unsigned reg) {
  setNumber(reg);
}

LogicalResult FloatRegisterOp::verify() {
  if (getNumber() > static_cast<unsigned>(RTGTestRegisters::f_5) ||
      getNumber() < static_cast<unsigned>(RTGTestRegisters::f_0))
    return emitOpError("'number' must be smaller than 10 and greater than 3");

  return success();
}

//===----------------------------------------------------------------------===//
// VectorRegisterOp
//===----------------------------------------------------------------------===//

unsigned VectorRegisterOp::getClassIndex() { return getNumber() - static_cast<unsigned>(RTGTestRegisters::v_0); }
APInt VectorRegisterOp::getClassIndexBinary() { return APInt(1, getClassIndex()); }

std::string VectorRegisterOp::getRegisterAssembly() {
  return "v" + std::to_string(getClassIndex());
}

llvm::BitVector VectorRegisterOp::getAllowedRegs() {
  llvm::BitVector allowed(static_cast<unsigned>(RTGTestRegisters::MAX));
  if (getNumber() < 0)
    return allowed.set(getNumber());
  return allowed.set(10,12);
}

unsigned VectorRegisterOp::getFixedReg() {
  if (getNumber() < 0)
    return ~0U;
  return getNumber();
}

void VectorRegisterOp::setFixedReg(unsigned reg) {
  setNumber(reg);
}

LogicalResult VectorRegisterOp::verify() {
  if (getNumber() > static_cast<unsigned>(RTGTestRegisters::v_1) ||
      getNumber() < static_cast<unsigned>(RTGTestRegisters::v_0))
    return emitOpError("'number' must be smaller than 12 and greater than 9");

  return success();
}

//===----------------------------------------------------------------------===//
// TableGen generated logic.
//===----------------------------------------------------------------------===//

// Provide the autogenerated implementation guts for the Op classes.
#define GET_OP_CLASSES
#include "circt/Dialect/RTGTest/IR/RTGTest.cpp.inc"
