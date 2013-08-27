//
// file: StmtChunk.h
// author: Kevin Lynx
//
#ifndef CPPWEE_STMTCHUNK_H
#define CPPWEE_STMTCHUNK_H

#include "f_decl.h"
#include "Stmt.h"
#include <list>

namespace Wee {
    class StmtFactory;

    class StmtChunk;
    typedef std::shared_ptr<StmtChunk> StmtChunkPtr;

    // StmtChunk contains a chunk of statements, these statements construct an execution flow,
    // it also manage the statements lifecycle.
    class StmtChunk {
    public:
        typedef std::list<StmtPtr> StmtList;

    public:
        StmtChunk(StmtFactory *fac, int id);

        // create a new statment by its type
        StmtRef create(StmtType type);

        // return the first statement of this chunk
        StmtRef head() const;

        // encode to byte buffer
        void encode(WriteBuffer &buf);

        // decode a chunk
        bool decode(ReadBuffer &buf);

        // clone the statements of this chunk, the cloned chunk is not managed by a factory.
        StmtChunkPtr clone() const;

        // view this chunk as a Graphviz file
        void view(const char *file);

        int id() const {
            return m_id;
        }

        const StmtList &getStmts() const {
            return m_stmts;
        }

    private:
        StmtList m_stmts;
        StmtRef m_head;
        StmtFactory *m_factory;
        int m_id;
    };
}

#endif // CPPWEE_STMTCHUNK_H
