#ifndef __REGISTERS_SET_H__
#define __REGISTERS_SET_H__

#include <vector>


/**
 * The array of defined registers aliases.
 */
std::vector<RegistersPair> registerSet = {
    //               alias    name
    RegistersPair( "$zero" , "$0"  ),
    RegistersPair( "$at"   , "$1"  ),
    RegistersPair( "$v0"   , "$2"  ),
    RegistersPair( "$v1"   , "$3"  ),
    RegistersPair( "$a0"   , "$4"  ),
    RegistersPair( "$a1"   , "$5"  ),
    RegistersPair( "$a2"   , "$6"  ),
    RegistersPair( "$a3"   , "$7"  ),
    RegistersPair( "$t0"   , "$8"  ),
    RegistersPair( "$t1"   , "$9"  ),
    RegistersPair( "$t2"   , "$10" ),
    RegistersPair( "$t3"   , "$11" ),
    RegistersPair( "$t4"   , "$12" ),
    RegistersPair( "$t5"   , "$13" ),
    RegistersPair( "$t6"   , "$14" ),
    RegistersPair( "$t7"   , "$15" ),
    RegistersPair( "$s0"   , "$16" ),
    RegistersPair( "$s1"   , "$17" ),
    RegistersPair( "$s2"   , "$18" ),
    RegistersPair( "$s3"   , "$19" ),
    RegistersPair( "$s4"   , "$20" ),
    RegistersPair( "$s5"   , "$21" ),
    RegistersPair( "$s6"   , "$22" ),
    RegistersPair( "$s7"   , "$23" ),
    RegistersPair( "$t8"   , "$24" ),
    RegistersPair( "$t9"   , "$25" ),
    RegistersPair( "$k0"   , "$26" ),
    RegistersPair( "$k1"   , "$27" ),
    RegistersPair( "$gp"   , "$28" ),
    RegistersPair( "$sp"   , "$29" ),
    RegistersPair( "$fp"   , "$30" ),
    RegistersPair( "$ra"   , "$31" )
};

#endif  // __REGISTERS_SET_H__