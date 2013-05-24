//
// file: Pipeline.cpp
// author: Kevin Lynx
//
#include <assert.h>
#include "Pipeline.h"

namespace Wee {
    Pipeline::Pipeline(int id, RunInstance *inst, StmtRef start, const ArgumentsPtr &arg) {
        m_id = id;
        m_ip = start;
        m_initArg = arg;
        m_first = true;
        m_inst = inst;
    } 

    StmtRet Pipeline::step(StmtRef prev, const ArgumentsPtr &arg) {
        if (!m_first) {
            Stmt::StmtRefs posts = m_ip->posts();
            if (posts.size() == 0)
                return StmtRet(StmtRet::END, nullptr);
            // spliter will be split first and dispatch to multiple pipelines
            assert(posts.size() == 1);
            m_ip = posts.front();
            m_ip->setContext(m_inst, this);
            return m_ip->execute(prev, arg);
        } else {
            m_first = false;
            m_ip->setContext(m_inst, this);
            return m_ip->execute(prev, m_initArg);
        }
    }
}
