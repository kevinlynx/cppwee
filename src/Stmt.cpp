//
// file: Stmt.cpp
// author: Kevin Lynx
//
#include <assert.h>
#include <algorithm>
#include "Stmt.h"

namespace Wee {
    void Stmt::setContext(RunInstance *inst, Pipeline *pipe) {
        m_inst = inst;
        m_pipe = pipe;
    }

    Stmt::WeakPtr Stmt::addPost(WeakPtr stmt) {
        assert(std::find(m_posts.begin(), m_posts.end(), stmt) == m_posts.end());
        if (std::find(m_posts.begin(), m_posts.end(), stmt) != m_posts.end())
            return nullptr;
        m_posts.push_back(stmt);
        stmt->addPrev(this);
        return stmt;
    }

    void Stmt::addPrev(WeakPtr stmt) {
        assert(std::find(m_prevs.begin(), m_prevs.end(), stmt) == m_prevs.end());
        if (std::find(m_prevs.begin(), m_prevs.end(), stmt) != m_prevs.end())
            return ;
        m_prevs.push_back(stmt);
    }

    Stmt::WeakPtr Stmt::next() {
        return m_posts.size() > 0 ? m_posts[0] : nullptr;
    }

    void Stmt::encode(WriteBuffer &buf) {

    }

    void Stmt::decode(ReadBuffer &buf) {
        
    }

    StmtRef Stmt::copy(StmtRef other) const {
        other->m_type = m_type;
        return other;
    }

    std::string Stmt::view() {
        std::string ret;
        char s[1024];
        sprintf(s, "Node0x%p [shape=record, label=\"{type: %d}\"];\n", this, type());
        ret = s;
        for (auto it = m_posts.begin(); it != m_posts.end(); ++it) {
            ret += (*it)->view();
            sprintf(s, "Node0x%p -> Node0x%p;\n", this, (*it));
            ret += s;
        }
        return ret;
    }
}
