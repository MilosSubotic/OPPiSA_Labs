#ifndef __PSEUDOINSTRUCTIONLIST_H__
#define __PSEUDOINSTRUCTIONLIST_H__

#include "assemblerLib.h"

#include <vector>


/**
 * The array of defined pseudo instructions
 */
const std::vector<OpCodeTablePair> g_pseudoInstructions = {
    //               symbol           replace instructions                           optype                 parNum
    OpCodeTablePair("text"  , OpCode(".org $SECTION_TEXT"                          , OT_PSEUDO , I_NONE , 0 , 0 , 0 )),
    OpCodeTablePair("data"  , OpCode(".org $SECTION_DATA"                          , OT_PSEUDO , I_NONE , 0 , 0 , 0 )),
    OpCodeTablePair("move"  , OpCode("or %1,%2,$0"                                 , OT_PSEUDO , I_NONE , 0 , 0 , 2 )),    // move $1, $2 -> or $1, $2, $0
    OpCodeTablePair("beqz"  , OpCode("beq %1,$0,%2"                                , OT_PSEUDO , I_NONE , 0 , 0 , 2 )),    // beqz $1, loop -> beq $1, $0, loop
    OpCodeTablePair("bneqz" , OpCode("bne %1,$0,%2"                                , OT_PSEUDO , I_NONE , 0 , 0 , 2 )),    // bneqz $1, loop -> bneq $1, $0, loop
    OpCodeTablePair("li"    , OpCode("lui %1,HI(%2)\nori %1,%1,LO(%2)"             , OT_PSEUDO , I_NONE , 0 , 0 , 2 )),    // li $1, 0x80000000 -> lui $1,0x8000\nori $1,0x0000
    OpCodeTablePair("la"    , OpCode("lui %1,HI(ADDR(%2))\nori %1,%1,LO(ADDR(%2))" , OT_PSEUDO , I_NONE , 0 , 0 , 2 )),    // li $1, $dataLabels{%2} -> lui $1,0x8000\n ori $1,$1,$dataLabels{%2}
    OpCodeTablePair("neg"   , OpCode("nor %1,%2,$0\naddi %1,%1,1"                  , OT_PSEUDO , I_NONE , 0 , 0 , 2 )),    // neg $1, $2   -> nor $1, $2, $0 , addi $1, $1, 1
    OpCodeTablePair("nori"  , OpCode("ori $1,$0,%3\nnor %1,%2,$1"                  , OT_PSEUDO , I_NONE , 0 , 0 , 3 )),    // not $1, $2   -> nor $1, $2, $0
    OpCodeTablePair("not"   , OpCode("nor %1,%2,$0"                                , OT_PSEUDO , I_NONE , 0 , 0 , 2 )),    // not $1, $2   -> nor $1, $2, $0
    OpCodeTablePair("bal"   , OpCode("bgezal $0,%1"                                , OT_PSEUDO , I_NONE , 0 , 0 , 1 )),
    OpCodeTablePair("b"     , OpCode("beq $0,$0,%1"                                , OT_PSEUDO , I_NONE , 0 , 0 , 1 )),
    OpCodeTablePair("bgt"   , OpCode("slt $1,%2,%1\nbne $1,$0,%3"                  , OT_PSEUDO , I_NONE , 0 , 0 , 3 )),
    OpCodeTablePair("blt"   , OpCode("slt $1,%1,%2\nbne $1,$0,%3"                  , OT_PSEUDO , I_NONE , 0 , 0 , 3 )),
    OpCodeTablePair("bge"   , OpCode("sub $1,%1,%2\nbgez $1,%3"                    , OT_PSEUDO , I_NONE , 0 , 0 , 3 )),
    OpCodeTablePair("ble"   , OpCode("sub $1,%2,%1\nbgez $1,%3"                    , OT_PSEUDO , I_NONE , 0 , 0 , 3 )),
    OpCodeTablePair("bgtu"  , OpCode("subu $1,%1,%2\nbgez $1,%3"                   , OT_PSEUDO , I_NONE , 0 , 0 , 3 )),
    OpCodeTablePair("mul"   , OpCode("mult %2,%3\nmflo %1"                         , OT_PSEUDO , I_NONE , 0 , 0 , 3 ))
};


#endif  // __PSEUDOINSTRUCTIONLIST_H__
