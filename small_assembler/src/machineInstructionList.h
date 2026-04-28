#ifndef __MACHINEINSTRUCTIONLIST_H__
#define __MACHINEINSTRUCTIONLIST_H__

#include "assemblerLib.h"

#include <vector>


/**
 * The array of defined machine instructions.
 */
const std::vector<OpCodeTablePair> g_machineInstructions = {
    //               symbol             symbol      optype format opcode funct  parNum
    OpCodeTablePair("add"     , OpCode("add"     , OT_MACHINE, R   , 0x0  , 0x20 , 3 )),
    OpCodeTablePair("addi"    , OpCode("addi"    , OT_MACHINE, I   , 0x8  , 0x0  , 3 )),
    OpCodeTablePair("addiu"   , OpCode("addiu"   , OT_MACHINE, I   , 0x9  , 0x0  , 3 )),
    OpCodeTablePair("addu"    , OpCode("addu"    , OT_MACHINE, R   , 0x0  , 0x21 , 3 )),
    OpCodeTablePair("and"     , OpCode("and"     , OT_MACHINE, R   , 0x0  , 0x24 , 3 )),
    OpCodeTablePair("andi"    , OpCode("andi"    , OT_MACHINE, I   , 0xC  , 0x0  , 3 )),
    OpCodeTablePair("beq"     , OpCode("beq"     , OT_MACHINE, B   , 0x4  , 0x0  , 3 )),
    OpCodeTablePair("bgez"    , OpCode("bgez"    , OT_MACHINE, BZ  , 0x1  , 0x1  , 2 )),
    OpCodeTablePair("bgezal"  , OpCode("bgezal"  , OT_MACHINE, BZ  , 0x1  , 0x11 , 2 )),
    OpCodeTablePair("bgtz"    , OpCode("bgtz"    , OT_MACHINE, BZ  , 0x7  , 0x0  , 2 )),
    OpCodeTablePair("blez"    , OpCode("blez"    , OT_MACHINE, BZ  , 0x6  , 0x0  , 2 )),
    OpCodeTablePair("bltz"    , OpCode("bltz"    , OT_MACHINE, BZ  , 0x1  , 0x0  , 2 )),
    OpCodeTablePair("bltzal"  , OpCode("bltzal"  , OT_MACHINE, BZ  , 0x1  , 0x10 , 2 )),
    OpCodeTablePair("bne"     , OpCode("bne"     , OT_MACHINE, B   , 0x5  , 0x0  , 3 )),
    OpCodeTablePair("div"     , OpCode("div"     , OT_MACHINE, MD  , 0x0  , 0x1A , 2 )),
    OpCodeTablePair("divu"    , OpCode("divu"    , OT_MACHINE, MD  , 0x0  , 0x1B , 2 )),
    OpCodeTablePair("j"       , OpCode("j"       , OT_MACHINE, J   , 0x2  , 0x0  , 1 )),
    OpCodeTablePair("jal"     , OpCode("jal"     , OT_MACHINE, J   , 0x3  , 0x0  , 1 )),
    OpCodeTablePair("jr"      , OpCode("jr"      , OT_MACHINE, JR  , 0x0  , 0x8  , 1 )),
    OpCodeTablePair("lb"      , OpCode("lb"      , OT_MACHINE, I   , 0x20 , 0x0  , 3 )),
    OpCodeTablePair("lui"     , OpCode("lui"     , OT_MACHINE, LUI , 0xF  , 0x0  , 2 )),
    OpCodeTablePair("lw"      , OpCode("lw"      , OT_MACHINE, I   , 0x23 , 0x0  , 3 )),
    OpCodeTablePair("mfhi"    , OpCode("mfhi"    , OT_MACHINE, MF  , 0x0  , 0x10 , 1 )),
    OpCodeTablePair("mflo"    , OpCode("mflo"    , OT_MACHINE, MF  , 0x0  , 0x12 , 1 )),
    OpCodeTablePair("mult"    , OpCode("mult"    , OT_MACHINE, MD  , 0x0  , 0x18 , 2 )),
    OpCodeTablePair("multu"   , OpCode("multu"   , OT_MACHINE, MD  , 0x0  , 0x19 , 2 )),
    OpCodeTablePair("nop"     , OpCode("nop"     , OT_MACHINE, N   , 0x0  , 0x0  , 0 )),
    OpCodeTablePair("nor"     , OpCode("nor"     , OT_MACHINE, R   , 0x0  , 0x27 , 3 )),
    OpCodeTablePair("or"      , OpCode("or"      , OT_MACHINE, R   , 0x0  , 0x25 , 3 )),
    OpCodeTablePair("ori"     , OpCode("ori"     , OT_MACHINE, I   , 0xD  , 0x0  , 3 )),
    OpCodeTablePair("sb"      , OpCode("sb"      , OT_MACHINE, I   , 0x28 , 0x0  , 3 )),
    OpCodeTablePair("sll"     , OpCode("sll"     , OT_MACHINE, S   , 0x0  , 0x0  , 3 )),
    OpCodeTablePair("sllv"    , OpCode("sllv"    , OT_MACHINE, SV  , 0x0  , 0x4  , 3 )),
    OpCodeTablePair("slt"     , OpCode("slt"     , OT_MACHINE, R   , 0x0  , 0x2A , 3 )),
    OpCodeTablePair("slti"    , OpCode("slti"    , OT_MACHINE, I   , 0xA  , 0x0  , 3 )),
    OpCodeTablePair("sltiu"   , OpCode("sltiu"   , OT_MACHINE, I   , 0xB  , 0x0  , 3 )),
    OpCodeTablePair("sltu"    , OpCode("sltu"    , OT_MACHINE, R   , 0x0  , 0x2B , 3 )),
    OpCodeTablePair("sra"     , OpCode("sra"     , OT_MACHINE, S   , 0x0  , 0x3  , 3 )),
    OpCodeTablePair("srav"    , OpCode("srav"    , OT_MACHINE, SV  , 0x0  , 0x7  , 3 )),
    OpCodeTablePair("srl"     , OpCode("srl"     , OT_MACHINE, S   , 0x0  , 0x2  , 3 )),
    OpCodeTablePair("srlv"    , OpCode("srlv"    , OT_MACHINE, SV  , 0x0  , 0x6  , 3 )),
    OpCodeTablePair("sub"     , OpCode("sub"     , OT_MACHINE, R   , 0x0  , 0x22 , 3 )),
    OpCodeTablePair("subu"    , OpCode("subu"    , OT_MACHINE, R   , 0x0  , 0x23 , 3 )),
    OpCodeTablePair("sw"      , OpCode("sw"      , OT_MACHINE, I   , 0x2B , 0x0  , 3 )),
    OpCodeTablePair("syscall" , OpCode("syscall" , OT_MACHINE, SC  , 0x0  , 0x09 , 0 )),
    OpCodeTablePair("xor"     , OpCode("xor"     , OT_MACHINE, R   , 0x0  , 0x26 , 3 )),
    OpCodeTablePair("xori"    , OpCode("xori"    , OT_MACHINE, I   , 0xE  , 0x0  , 3 ))
};

#endif  // __MACHINEINSTRUCTIONLIST_H__
