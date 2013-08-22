//
// file: IfStmt.h
// author: Kevin Lynx
//
#ifndef CPPWEE_IFSTMT_H
#define CPPWEE_IFSTMT_H

#include "Stmt.h"

namespace Wee {
    enum { COND_ARGNULL };

    class Cond {
    public:
        Cond(int t) : m_type(t) {
        }

        virtual ~Cond() { }

        virtual bool check(ArgumentsPtr args) = 0;

        static Cond *decode(ReadBuffer &buf);

        static void encode(const Cond *cond, WriteBuffer &buf);

        static Cond *clone(const Cond *cond);

    private:
        int m_type;
    };

    class ArgNullCond : public Cond {
    public:
        ArgNullCond() : Cond(COND_ARGNULL) {
        }

        virtual bool check(ArgumentsPtr args);
    };

    class IfStmt : public Stmt {
    public:
        IfStmt() : Stmt(ST_IF) {
            m_next = nullptr;
            m_cond = nullptr;
        }

        ~IfStmt();

        virtual StmtRet execute(StmtRef prev, ArgumentsPtr args);

        virtual WeakPtr next();

        virtual void encode(WriteBuffer &buf);

        virtual void decode(ReadBuffer &buf);

        virtual StmtRef copy(StmtRef other) const;

        // `IfStmt' will delete `cond' in destructor
        void setCond(Cond *cond);

    private:
        WeakPtr m_next;
        Cond *m_cond;
    };
}

#endif // CPPWEE_GOTOSTMT_H
