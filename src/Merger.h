//
// file: Merger.h
// author: Kevin Lynx
//
#include <map>
#include "Stmt.h"

namespace Wee {
    // a merger can merge multiple input arguments into one single return value,
    // it will hangup the current pipeline until get all arguments.
    class Merger : public Stmt {
        typedef std::map<unsigned, ArgumentsPtr> ArgsTable;
    public:
        Merger() : Stmt(ST_MERGER) {
        }

        virtual StmtRet execute(StmtRef prev, ArgumentsPtr args);

    private:
        ArgsTable m_args;
    };
}
