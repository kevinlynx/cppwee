//
// file: StmtChunk.cpp
// author: Kevin Lynx
//
#include <assert.h>
#include <algorithm>
#include <map>
#include "StmtChunk.h"
#include "StmtFactory.h"
#include "Buffer.h"

namespace Wee {
    StmtChunk::StmtChunk(StmtFactory *fac, int id) {
        m_head = nullptr;
        m_factory = fac;
        m_id = id;
    }

    StmtRef StmtChunk::create(StmtType type) {
        assert(m_factory);
        StmtPtr stmt = m_factory->create(type);
        assert(stmt);
        m_stmts.push_back(stmt);
        m_head == nullptr ? m_head = stmt.get() : nullptr;
        return stmt.get();
    }

    StmtRef StmtChunk::head() const {
        return m_head;
    }

    StmtChunkPtr StmtChunk::clone() const {
        StmtChunkPtr chunk(new StmtChunk(m_factory, -1));
        int id = 0;
        std::map<StmtRef, int> stmtids; // old statements map to ids
        std::map<int, StmtRef> idstmts; // new statements
        typedef std::vector<int> IDs;
        std::map<int, IDs> prevIDs; // statement to previous ids
        // build id table
        for (auto it = m_stmts.begin(); it != m_stmts.end(); ++it, ++id) {
            StmtRef thisstmt = (*it).get();
            StmtRef newstmt = chunk->create(thisstmt->type());
            thisstmt->copy(newstmt);
            stmtids[thisstmt] = id;
            idstmts[id] = newstmt;
        }
        // build previous ids table
        for (auto it = m_stmts.begin(); it != m_stmts.end(); ++it, ++id) {
            StmtRef thisstmt = (*it).get();
            const Stmt::StmtRefs &prevs = thisstmt->prevs();
            IDs ids;
            for (auto pit = prevs.begin(); pit != prevs.end(); ++pit) {
                assert(stmtids.find(*pit) != stmtids.end());
                ids.push_back(stmtids[*pit]); 
            }
            assert(stmtids.find(thisstmt) != stmtids.end());
            prevIDs[stmtids[thisstmt]] = ids;
        }
        // build the new statements relationship
        id = 0;
        for (auto it = chunk->m_stmts.begin(); it != chunk->m_stmts.end(); ++it, ++id) {
            StmtRef newstmt = (*it).get();
            assert(prevIDs.find(id) != prevIDs.end());
            const IDs &ids = prevIDs[id];
            for (auto idit = ids.begin(); idit != ids.end(); ++idit) {
                int pid = *idit;
                assert(idstmts.find(pid) != idstmts.end());
                idstmts[pid]->addPost(newstmt);
            }
        }
        return chunk;
    }

    void StmtChunk::encode(WriteBuffer &buf) {
        assert(m_head);
        std::map<StmtRef, int> ids;
        int id = 1;
        auto addids = [&buf, &ids] (StmtRef ref) -> void {
            auto it = ids.find(ref);
            assert(it != ids.end());
            buf.write(it->second);
        };
        buf.write(m_stmts.size());
        for (auto it = m_stmts.begin(); it != m_stmts.end(); ++it) {
            auto stmt = (*it).get();
            buf.write(stmt->type());
            buf.write(id);
            stmt->encode(buf);
            ids[stmt] = id++;
        }
        for (auto it = m_stmts.begin(); it != m_stmts.end(); ++it) {
            auto prevs = (*it)->prevs();
            buf.write((unsigned char) prevs.size());            
            std::for_each(prevs.begin(), prevs.end(), addids);
        }
    }

    bool StmtChunk::decode(ReadBuffer &buf) {
        m_stmts.clear();
        std::map<int, StmtRef> stmtids;
        size_t size = buf.read<size_t>();
        std::vector<StmtRef> stmtRefs;
        for (size_t i = 0; i < size; ++i) { 
            StmtType type = buf.read<StmtType>();
            StmtRef stmt = create(type);
            if (!stmt) return false;
            int id = buf.read<int>();
            stmt->decode(buf);
            stmtids[id] = stmt;
            stmtRefs.push_back(stmt);
        }
        for (size_t i = 0; i < size; ++i) {
            unsigned char prev = buf.read<unsigned char>();
            auto stmt = stmtRefs[i];
            for (auto j = 0; j < prev; ++j) {
                int id = buf.read<int>();
                auto it = stmtids.find(id);
                if (it == stmtids.end()) return false;
                auto prevStmt = it->second;
                prevStmt->addPost(stmt);
            }
        }
        return true;
    }

    void StmtChunk::view(const char *file) {
        std::string s = "digraph unnamed {\n";
        s += m_head->view();
        s += "}\n";
        FILE *fp = fopen(file, "w");
        if (!fp) return ;
        fwrite(s.c_str(), s.size(), 1, fp);
        fclose(fp);
    }
}
