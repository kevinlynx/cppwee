//
// file: f_decl.h
// author: Kevin Lynx
//
#ifndef CPPWEE_FDECL_H
#define CPPWEE_FDECL_H

#include <memory>

namespace Wee {
    class Stmt;
    typedef Stmt* StmtRef;
    typedef std::shared_ptr<Stmt> StmtPtr;
    class ArgumentList;
    typedef std::shared_ptr<ArgumentList> ArgumentsPtr;
    class Pipeline;
    typedef std::shared_ptr<Pipeline> PipelinePtr;
    class RunInstance;
    typedef std::shared_ptr<RunInstance> RunInstancePtr;
    class StmtChunk;
    typedef std::shared_ptr<StmtChunk> StmtChunkPtr;
}
class ReadBuffer;
class WriteBuffer;

#pragma warning(disable: 4996)
#endif // CPPWEE_FDECL_H
