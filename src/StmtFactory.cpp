//
// file: StmtFactory.cpp
// author: Kevin Lynx
//
#include "StmtFactory.h"
#include "Stmt.h"
#include "Spliter.h"
#include "Merger.h"
#include "GotoStmt.h"
#include "IfStmt.h"
#include <assert.h>

namespace Wee {
    StmtFactory::StmtFactory() {
        m_idSeed = 1;
        reg(ST_MERGER, STMT_NEW_CREATOR(Merger));
        reg(ST_SPLITER, STMT_NEW_CREATOR(Spliter));
        reg(ST_GOTO, STMT_NEW_CREATOR(GotoStmt));
        reg(ST_IF, STMT_NEW_CREATOR(IfStmt));
    }

    StmtFactory::~StmtFactory() {
    }

    void StmtFactory::reg(StmtType type, Creator c) {
        m_creators[type] = c;
    }

    StmtPtr StmtFactory::create(StmtType type) {
        auto it = m_creators.find(type);
        if (it == m_creators.end()) return nullptr;
        auto c = it->second;
        StmtPtr stmt = c();
        assert(stmt->type() == type);
        return stmt;
    }

    StmtChunkPtr StmtFactory::createChunk() {
        StmtChunkPtr chunk(new StmtChunk(this, m_idSeed));
        m_chunks[m_idSeed++] = chunk;
        return chunk;
    }

    StmtChunkPtr StmtFactory::findChunk(int id) const {
        auto it = m_chunks.find(id);
        return it == m_chunks.end() ? nullptr : it->second;
    }

    bool StmtFactory::destroyChunk(int id) {
        m_chunks.erase(id);
        return true;
    }
}
