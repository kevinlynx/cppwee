//
// file: DataStmt.h
// author: Kevin Lynx
//
#ifndef CPPWEE_DATA_STMT_H
#define CPPWEE_DATA_STMT_H

#include "Stmt.h"

namespace Wee {
    // `DataStmt' is only used to encode a base statement with different types.
    template <int Type>
    class DataStmt : public Stmt {
    public:
        DataStmt() : Stmt((StmtType) Type) {
        } 

        virtual StmtRet execute(WeakPtr prev, ArgumentsPtr args) {
            return StmtRet(StmtRet::CONTINUE, args);
        }
    };
}

#endif // CPPWEE_DATA_STMT_H
