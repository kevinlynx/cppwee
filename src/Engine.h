//
// file: Engine.h
// author: Kevin Lynx
//
#ifndef CPPWEE_ENGINE_H
#define CPPWEE_ENGINE_H

#include "Stmt.h"
#include "Spliter.h"
#include "Merger.h"
#include "GotoStmt.h"
#include "IfStmt.h"
#include "StmtFactory.h"
#include "RunInstance.h"
#include "Pipeline.h"
#include "StmtChunk.h"
#include "DataStmt.h"

namespace Wee {
    class Engine {
        typedef std::map<int, RunInstancePtr> Instances;
    public:
        // run a new instance
        bool run(const StmtChunkPtr &chunk, ArgumentsPtr arg, RunInstance::Callback cb);

        // resume halted run instance
        bool resume(int id, int pipe, ArgumentsPtr arg);

        static Engine &instance();

    private:
        Engine();
        Engine(Engine &);
        Engine &operator= (const Engine &);

        void remove(int id);

    private:
        Instances m_insts;
        int m_idSeed;
    };
}

#endif // CPPWEE_ENGINE_H
