//
// file: Pipeline.h
// author: Kevin Lynx
//
#ifndef CPPWEE_PIPELINE_H
#define CPPWEE_PIPELINE_H

#include "Stmt.h"

namespace Wee {
    class RunInstance;
    // `Pipeline' is a single execution flow, there will be multiple pipelines in 
    // a `RunInstance' in a parallel statement chunk.
    class Pipeline {
    public:
        Pipeline(int id, RunInstance *inst, StmtRef start, const ArgumentsPtr &arg);

        StmtRet step(StmtRef prev, const ArgumentsPtr &arg);

        StmtRef IP() {
            return m_ip;
        }

        int id() const {
            return m_id;
        }

    private:
        int m_id;
        StmtRef m_ip;
        ArgumentsPtr m_initArg;
        bool m_first;
        RunInstance *m_inst;
    };
    typedef std::shared_ptr<Pipeline> PipelinePtr;
}

#endif // CPPWEE_PIPELINE_H

