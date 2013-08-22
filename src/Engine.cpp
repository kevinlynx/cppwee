//
// file: Engine.cpp
// author: Kevin Lynx
//
#include <algorithm>
#include "Engine.h"
#include <assert.h>

namespace Wee {
    Engine::Engine() {
        m_idSeed = 0;
    }

    bool Engine::run(const StmtChunkPtr &chunk, ArgumentsPtr arg, RunInstance::Callback cb) {
        RunInstance::Environment env;
        return run(chunk, arg, cb, env);
    }

    bool Engine::run(const StmtChunkPtr &chunk, ArgumentsPtr arg, RunInstance::Callback cb,
        const RunInstance::Environment &env) {
        int id = ++m_idSeed;
        auto adapterCB = [this, cb, id] (ChunkRet cr, ArgumentsPtr ret) -> void {
            cb(cr, ret);
            // NOTE: here may cause to access an invalid RunInstnace object after this
            // callback returns.
            if (cr.code != RC_HALT) // error/success will remove the instance
                this->remove(id);
        };
        RunInstancePtr inst(new RunInstance(id, chunk, arg, adapterCB, env));
        m_insts[id] = inst;
        bool ret = inst->run();
        return ret;
    }

    bool Engine::resume(int id, int pipe, ArgumentsPtr arg) {
        auto it = m_insts.find(id);
        assert(it != m_insts.end());
        if (it == m_insts.end()) return false;
        auto inst = it->second;
        inst->run(pipe, arg);
        return true;
    }

    Engine &Engine::instance() {
        static Engine s_inst;
        return s_inst;
    }

    void Engine::remove(int id) {
        m_insts.erase(id);
    }
}
