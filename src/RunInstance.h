//
// file: RunInstance.h
// author: Kevin Lynx
//
#ifndef CPPWEE_RUNINSTANCE_H
#define CPPWEE_RUNINSTANCE_H

#include "f_decl.h"
#include <list>
#include <functional>
#include <tuple>
#include <map>
#include <string>

namespace Wee {
    // a whole statement chunk execution result
    enum {
        RC_SUCCESS,
        RC_INVALID_STMT,
        RC_HALT,
        RC_UNKNOWN,
        RC_USER = 0x0f
    };

    // the exectuion result for a chunk
    struct ChunkRet {
        int code;
        union {
            struct Halt {
                int pipe;
                int inst;
            } halt;
        } args;

        explicit ChunkRet(int c) : code(c) {
        }
    };

    // represents a run instance of StmtChunk
    class RunInstance {
    public:
        typedef std::list<PipelinePtr> Pipelines;
        typedef std::function<void (ChunkRet, ArgumentsPtr ret)> Callback;
        typedef std::list<StmtRef> PrevStack;
        typedef std::map<std::string, std::string> Environment;

    public:
        // it will clone a new chunk from `chunk'
        RunInstance(int id, const StmtChunkPtr &chunk, ArgumentsPtr arg, Callback cb,
            const Environment &env);

        // return true to indicate the run process has been done
        bool run();

        // to resume the halt statment chunk
        bool run(int pipe, ArgumentsPtr arg);

        // called by hangup statements
        bool run(Pipeline *pipe, ArgumentsPtr arg);

        // called by failed statments
        void fail(int code);

        Environment &env();

    private:
        bool doRun(ArgumentsPtr arg, StmtRef prev);

        StmtRef popSplitStmt();

        void onHalt(Pipeline *pipe, ArgumentsPtr arg);

        PipelinePtr newPipeline(StmtRef head, ArgumentsPtr args);

    private:
        int m_id;
        int m_pipeIdSeed;
        Pipelines m_pipes;
        Pipelines m_hangupPipes;
        PrevStack m_prevS;
        Callback m_cb;
        StmtChunkPtr m_chunk;
        Environment m_env;
    };
    typedef std::shared_ptr<RunInstance> RunInstancePtr;
}

#endif // CPPWEE_RUNINSTANCE_H
