//
// file: Spliter.h
// author: Kevin Lynx
//
#include <map>
#include "Stmt.h"

namespace Wee {
    // split one argument to multiple return values
    class Spliter : public Stmt {
    public:
        Spliter() : Stmt(ST_SPLITER) {
        }

        virtual StmtRet execute(StmtRef prev, ArgumentsPtr args);
    };
}
