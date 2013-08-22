//
// file: GotoStmt.h
// author: Kevin Lynx
//
#ifndef CPPWEE_GOTOSTMT_H
#define CPPWEE_GOTOSTMT_H

#include "Stmt.h"

namespace Wee {
    class GotoStmt : public Stmt {
    public:
        GotoStmt() : Stmt(ST_GOTO) {
        }

        virtual StmtRet execute(StmtRef prev, ArgumentsPtr args) {
            return StmtRet(args);
        }
    };
}

#endif // CPPWEE_GOTOSTMT_H
