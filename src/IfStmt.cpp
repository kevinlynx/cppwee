//
// file: IfStmt.cpp
// author: Kevin Lynx
//
#include <assert.h>
#include "IfStmt.h"
#include "Buffer.h"

namespace Wee {
    Cond *Cond::decode(ReadBuffer &buf) {
        int t = buf.read<int>();
        if (t == COND_ARGNULL)
            return new ArgNullCond();
        return nullptr;
    }

    void Cond::encode(const Cond *cond, WriteBuffer &buf) {
        buf.write(cond->m_type); 
    }

    Cond *Cond::clone(const Cond *cond) {
        if (cond->m_type == COND_ARGNULL)
            return new ArgNullCond();
        return nullptr;
    }

    bool ArgNullCond::check(ArgumentsPtr args) {
        return args->isNull();
    }

    IfStmt::~IfStmt() {
        assert(m_cond);
        delete m_cond;
    }

    StmtRet IfStmt::execute(StmtRef prev, ArgumentsPtr args) {
        assert(m_cond);
        assert(m_posts.size() == 2);
        m_next = m_cond->check(args) ? m_posts[1] : m_posts[0];
        return StmtRet(args);
    }

    Stmt::WeakPtr IfStmt::next() {
        assert(m_next);
        return m_next;
    }

    void IfStmt::encode(WriteBuffer &buf) {
        Cond::encode(m_cond, buf);
    }

    void IfStmt::decode(ReadBuffer &buf) {
        m_cond = Cond::decode(buf);
        assert(m_cond);
    }

    StmtRef IfStmt::copy(StmtRef other) const {
        IfStmt *oifs = (IfStmt*) other;
        Cond *cond = Cond::clone(m_cond);
        oifs->setCond(cond);
        return other;
    }

    void IfStmt::setCond(Cond *cond) {
        assert(cond);
        m_cond = cond;
    }
}
