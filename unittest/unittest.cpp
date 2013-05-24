//
// file: unittest.cpp
// brief: unit tests for cppwee
// author: Kevin Lynx
//
#include <stdio.h>
#include <gtest/gtest.h>
#include "../src/Engine.h"
#include "../src/Buffer.h"

namespace Wee {
    enum { ST_STMT1 = ST_USER, ST_STMT2, ST_STMT3 };
}

class WeeTest : public testing::Test {
protected:
    virtual void SetUp() {
        m_fac.reg(Wee::ST_STMT1, STMT_NEW_CREATOR(Wee::DataStmt<Wee::ST_STMT1>));
        m_fac.reg(Wee::ST_STMT2, STMT_NEW_CREATOR(Wee::DataStmt<Wee::ST_STMT2>));
        m_fac.reg(Wee::ST_STMT3, STMT_NEW_CREATOR(Wee::DataStmt<Wee::ST_STMT3>));
        m_chunk = m_fac.createChunk();
        createStmts();
    }

    void createStmts() {
        auto chunk = m_chunk;
        Wee::StmtRef stmt = chunk->create(Wee::ST_STMT1);
        ASSERT_NE(stmt, nullptr);
        Wee::StmtRef spliter = chunk->create(Wee::ST_SPLITER);
        ASSERT_NE(spliter, nullptr);
        Wee::StmtRef merger = chunk->create(Wee::ST_MERGER);
        ASSERT_NE(merger, nullptr);
        stmt->addPost(spliter)->
            addPost(chunk->create(Wee::ST_STMT2))->
            addPost(merger)->
            addPost(chunk->create(Wee::ST_STMT3));
        spliter->addPost(chunk->create(Wee::ST_STMT1))->
            addPost(merger);
    }

    Wee::StmtFactory m_fac;
    Wee::StmtChunkPtr m_chunk;
};

TEST_F(WeeTest, chunk_create) {
    auto chunk = m_chunk;
    ASSERT_NE(chunk, nullptr);
    EXPECT_NE(chunk->head(), nullptr);
}

TEST_F(WeeTest, chunk_clone) {
    auto chunk = m_chunk->clone();
    ASSERT_NE(chunk, nullptr);
    ByteBuffer buf;
    VectorWriteBuffer vbuf(buf);
    m_chunk->encode(vbuf);
    ByteBuffer buf2;
    VectorWriteBuffer vbuf2(buf2);
    chunk->encode(vbuf2);
    EXPECT_EQ(buf, buf2);
}

TEST_F(WeeTest, chunk_encode) {
    ByteBuffer buf;
    VectorWriteBuffer vbuf(buf);
    m_chunk->encode(vbuf);
    auto chunk = m_fac.createChunk();
    chunk->decode(VectorReadBuffer(buf));
    ByteBuffer buf2;
    VectorWriteBuffer vbuf2(buf2);
    chunk->encode(vbuf2);
    EXPECT_EQ(buf, buf2);
}

TEST_F(WeeTest, factory) {
    auto chunk = m_fac.createChunk();
    EXPECT_EQ(m_fac.findChunk(m_chunk->id()), m_chunk);
    EXPECT_NE(chunk->id(), m_chunk->id());
    EXPECT_EQ(m_fac.findChunk(chunk->id()), chunk);
    m_fac.destroyChunk(chunk->id());
    EXPECT_EQ(m_fac.findChunk(chunk->id()), nullptr);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
