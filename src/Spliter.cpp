//
// file: Spliter.cpp
// author: Kevin Lynx
//
#include "Spliter.h"

namespace Wee {
    StmtRet Spliter::execute(StmtRef prev, ArgumentsPtr args) {
        return StmtRet(StmtRet::SPLIT, args);
    }
}
