//
// file: StmtFactory.h
// author: Kevin Lynx
//
#ifndef CPPWEE_STMTFACTORY_H
#define CPPWEE_STMTFACTORY_H

#include <list>
#include <map>
#include <functional>
#include "f_decl.h"
#include "StmtChunk.h"

namespace Wee {
    // manage statement chunks
    class StmtFactory {
    public:
        typedef std::shared_ptr<Stmt> StmtPtr;
        typedef std::map<int, StmtChunkPtr> StmtChunkTable;
        typedef std::function<StmtPtr (void)> Creator;
        typedef std::map<StmtType, Creator> Creators;

    public:
        StmtFactory();

        virtual ~StmtFactory();

        // register a statement creator
        void reg(StmtType type, Creator c);

        // create a chunk and manage the chunk in this factory
        StmtChunkPtr createChunk();

        // find a chunk by its id
        StmtChunkPtr findChunk(int id) const;

        bool destroyChunk(int id);

    private:
        StmtPtr create(StmtType type);
        friend class StmtChunk;

    private:
        StmtChunkTable m_chunks;
        Creators m_creators;
        int m_idSeed;
    };
}

#define STMT_NEW_CREATOR(Class) \
    [] () -> Wee::StmtFactory::StmtPtr { \
        return Wee::StmtFactory::StmtPtr(new Class()); \
    }

#endif // CPPWEE_STMTFACTORY_H
