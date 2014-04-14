//===--- SILValue.cpp - Implementation for SILValue -----------------------===//
//
// This source file is part of the Swift.org open source project
//
// Copyright (c) 2014 - 2015 Apple Inc. and the Swift project authors
// Licensed under Apache License v2.0 with Runtime Library Exception
//
// See http://swift.org/LICENSE.txt for license information
// See http://swift.org/CONTRIBUTORS.txt for the list of Swift project authors
//
//===----------------------------------------------------------------------===//

#include "swift/SIL/SILValue.h"
#include "swift/SIL/SILInstruction.h"

using namespace swift;

void SILValue::replaceAllUsesWith(SILValue V) {
  assert(*this != V && "Cannot RAUW a value with itself");
  assert(getType() == V.getType() && "Invalid type");
  while (!use_empty())
    (**use_begin()).set(V);
}

SILValue SILValue::stripCasts() {
  SILValue V = *this;

  while (true) {
    switch (V->getKind()) {
    case ValueKind::UpcastInst:
    case ValueKind::AddressToPointerInst:
    case ValueKind::PointerToAddressInst:
    case ValueKind::RefToObjectPointerInst:
    case ValueKind::ObjectPointerToRefInst:
    case ValueKind::RefToRawPointerInst:
    case ValueKind::RawPointerToRefInst:
    case ValueKind::UnconditionalCheckedCastInst: {
      auto *I = cast<SILInstruction>(V.getDef());
      V = I->getOperand(0);
      assert(V != I && "Instruction cannot be one of its own operands!");
      continue;
    }
    default:
      return V;
    }
  }
}

SILValue SILValue::stripAddressProjections() {
  SILValue V = *this;

  while (true) {
    switch (V->getKind()) {
    case ValueKind::StructElementAddrInst:
    case ValueKind::TupleElementAddrInst:
    case ValueKind::RefElementAddrInst: {
      auto *I = cast<SILInstruction>(V.getDef());
      V = I->getOperand(0);
      assert(V != I && "Instruction cannot be one of its own operands!");
      continue;
    }
    default:
      return V;
    }
  }
}

SILValue SILValue::stripAggregateProjections() {
  SILValue V = *this;

  while (true) {
    switch (V->getKind()) {
    case ValueKind::StructExtractInst:
    case ValueKind::TupleExtractInst: {
      auto *I = cast<SILInstruction>(V.getDef());
      V = I->getOperand(0);
      assert(V != I && "Instruction cannot be one of its own operands!");
      continue;
    }
    default:
      return V;
    }
  }
}

SILValue SILValue::stripIndexingInsts() {
  SILValue V = *this;
  while (true) {
    if (!isa<IndexingInst>(V.getDef()))
      return V;
    auto *I = cast<IndexingInst>(V);
    V = I->getBase();
    assert(V != I && "Instruction cannot be one of its own operands!");
  }
}
