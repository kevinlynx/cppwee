//
// file: RunInstance.cpp
// author: Kevin Lynx
//
#include <assert.h>
#include <algorithm>
#include "RunInstance.h"
#include "Pipeline.h"
#include "StmtChunk.h"
#include "Stmt.h"

namespace Wee {
    RunInstance::RunInstance(int id, const StmtChunkPtr &chunk, ArgumentsPtr arg, Callback cb,
        const Environment &env) :
        m_cb(cb),
        m_env(env) {
        m_id = id;
        m_pipeIdSeed = 0;
        m_chunk = chunk->clone();
        m_pipes.push_back(newPipeline(m_chunk->head(), arg));
    }

    bool RunInstance::run() {
        // the initial start require only 1 pipeline
        assert(m_pipes.size() == 1);
        return doRun(nullptr, nullptr);
    }

    // the first run or called by a hangup stmt resuming callback
    bool RunInstance::doRun(ArgumentsPtr arg, StmtRef prev) {
        auto pipe = m_pipes.front();
        bool halt = false;
        // every pipeline will run to the end or reach a hangup stmt
        while (!m_pipes.empty()) {
            StmtRet ret = pipe->step(prev, arg);
            if (ret.code() == StmtRet::SPLIT) { // require the posts count = the results count
                StmtRef ip = pipe->IP();
                auto arglist = ret.args();
                assert(arglist);
                auto argarray = arglist->split();
                assert(argarray.size() == ip->posts().size());
                // destroy this pipeline and create new piplines
                m_pipes.pop_front();
                auto posts = ip->posts();
                for (size_t i = 0; i < posts.size(); ++i) {
                    auto stmt = posts[i];
                    auto arg = argarray[i];
                    m_pipes.push_front(newPipeline(stmt, arg));
                    // the `ip' here will be the `prev' stmt of these new pipelines
                    m_prevS.push_front(ip);
                }
                pipe = m_pipes.front();
                prev = popSplitStmt();
            } else if (ret.code() == StmtRet::CONTINUE) {
                // the most simple case, just keep going
                prev = pipe->IP(); 
                arg = ret.args();
            } else if (ret.code() == StmtRet::HANGUP) {
                // the stmt will resume the pipeline by itself
                m_pipes.pop_front();
                m_hangupPipes.push_back(pipe);
                // back the workflow and run a new pipeline
                if (m_pipes.empty()) break;
                pipe = m_pipes.front();
                prev = popSplitStmt();
                arg = nullptr;
            } else if (ret.code() == StmtRet::MERGE_WAIT) {
                // this pipeline has been done
                m_pipes.pop_front();
                // there must be other pipelines or in the hangup pipelines
                assert(m_pipes.size() > 0 || m_hangupPipes.size() > 0);
                if (m_pipes.size() > 0) {
                    // try to execute the other pipeline
                    pipe = m_pipes.front();
                    prev = popSplitStmt();
                }
                arg = nullptr;
            } else if (ret.code() == StmtRet::HALT) {
                m_pipes.pop_front();
                m_hangupPipes.push_back(pipe);
                halt = true;
                onHalt(pipe.get(), ret.args());
                break;
            } else if (ret.code() == StmtRet::END) {
                m_pipes.pop_front();
                assert(m_pipes.empty() && m_hangupPipes.empty());
                break;
            }
        }
        assert(halt || m_pipes.empty());
        assert(halt || m_prevS.empty());
        bool done = m_hangupPipes.empty();
        if (done)
            m_cb(ChunkRet(RC_SUCCESS), arg);
        return done;
    }

    bool RunInstance::run(Pipeline *pipe, ArgumentsPtr arg) {
        auto it = std::find_if(m_hangupPipes.begin(), m_hangupPipes.end(),
            [pipe] (const PipelinePtr &other) -> bool {
                return other.get() == pipe;
        });
        assert(it != m_hangupPipes.end());
        if (it == m_hangupPipes.end()) return false;
        auto ptr = *it;
        m_hangupPipes.erase(it);
        // the hangup pipeline stay the same state which means the `IP' still points to the hangup stmt 
        // TODO: push_front ? to continue the same pipeline
        m_pipes.push_back(ptr);
        return doRun(arg, ptr->IP());
    }

    bool RunInstance::run(int pipe, ArgumentsPtr arg) {
        auto it = std::find_if(m_hangupPipes.begin(), m_hangupPipes.end(),
            [pipe] (const PipelinePtr &other) -> bool {
                return other->id() == pipe;
        });
        if (it == m_hangupPipes.end()) {
            m_cb(ChunkRet(RC_UNKNOWN), nullptr); // fatal error
            return false;
        }
        auto ptr = *it;
        m_hangupPipes.erase(it);
        // continue the same pipeline
        m_pipes.push_front(ptr);
        return doRun(arg, ptr->IP());
    }

    void RunInstance::fail(int code) {
        m_cb(ChunkRet(code), nullptr);
    }

    StmtRef RunInstance::popSplitStmt() {
        assert(!m_prevS.empty());
        StmtRef ref = m_prevS.front();
        m_prevS.pop_front();
        return ref;
    }

    void RunInstance::onHalt(Pipeline *pipe, ArgumentsPtr args) {
        ChunkRet cr(RC_HALT);
        cr.args.halt.inst = m_id;
        cr.args.halt.pipe = pipe->id();
        m_cb(cr, args);
    }

    PipelinePtr RunInstance::newPipeline(StmtRef head, ArgumentsPtr args) {
        return PipelinePtr(new Pipeline(++m_pipeIdSeed, this, head, args));
    }

    RunInstance::Environment &RunInstance::env() {
        return m_env;
    }
}
