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
            Stmt::WeakPtr next = m_ip->next();
            if (next == nullptr) 
                return StmtRet(StmtRet::END, nullptr);
            m_ip = next;
            m_ip->setContext(m_inst, this);
            return m_ip->execute(prev, arg);
        } else {
            m_first = false;
            m_ip->setContext(m_inst, this);
            return m_ip->execute(prev, m_initArg);
        }
    }
}
