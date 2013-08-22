//
// file: test.cpp
// author: Kevin Lynx
// brief: a simple test for cppwee
//
#include "../src/Engine.h"
#include "../src/Buffer.h"
#include <stdio.h>
#include <string>
#include <assert.h>

using namespace Wee;

typedef std::string MyArg;

enum { STMT_HELLO = ST_USER, STMT_MULTI, STMT_NULL, 
    STMT_HELLO2, STMT_INC };

class MyArguments : public ArgumentList {
public:
    void add(const MyArg &arg) {
        m_pack.push_back(arg);
    }

    std::string getAsStr() const {
        std::string s;
        for (auto it = m_pack.begin(); it != m_pack.end(); ++it) {
            s += (*it);
        }
        return s;
    }

    virtual ArgArray split() {
        ArgArray array;
        for (auto it = m_pack.begin(); it != m_pack.end(); ++it) {
            MyArguments *myarg = new MyArguments();
            myarg->add(*it);
            array.push_back(Ptr(myarg));
        }
        return array;
    }
    
    virtual ArgumentsPtr merge(const ArgArray &args) {
        auto *arg = new MyArguments();
        for (auto it = args.begin(); it != args.end(); ++it) {
            MyArguments *myarg = (MyArguments*) (*it).get();
            arg->m_pack.push_back(myarg->m_pack.front());
        }
        return ArgumentsPtr(arg);
    }

    virtual bool isNull() const {
        return m_pack.size() == 0;
    }

private:
    std::vector<MyArg> m_pack;
};

class HelloStmt : public Stmt {
public:
    HelloStmt() : Stmt(STMT_HELLO) {
    }

    virtual StmtRet execute(StmtRef prev, ArgumentsPtr args) {
        MyArguments *myarg = (MyArguments*) args.get();
        std::string s = ">" + myarg->getAsStr();
        printf("%s\n", s.c_str());
        myarg = new MyArguments();
        myarg->add(s);
        return StmtRet(StmtRet::CONTINUE, ArgumentsPtr(myarg));
    }
};

class Hello2Stmt : public Stmt {
public:
    Hello2Stmt() : Stmt(STMT_HELLO2) {
    }

    virtual StmtRet execute(StmtRef prev, ArgumentsPtr args) {
        MyArguments *myarg = (MyArguments*) args.get();
        std::string s = "=" + myarg->getAsStr();
        printf("%s\n", s.c_str());
        myarg = new MyArguments();
        myarg->add(s);
        return StmtRet(StmtRet::CONTINUE, ArgumentsPtr(myarg));
    }
};

int g_inc = 0;

class IncStmt : public Stmt {
public:
    IncStmt() : Stmt(STMT_INC) {
    }

    virtual StmtRet execute(StmtRef prev, ArgumentsPtr args) {
        auto *myarg = new MyArguments();
        if (g_inc++ > 5) {
            printf("loop end\n");
            return StmtRet(ArgumentsPtr(myarg));
        }
        printf("IncStmt\n");
        myarg->add("s");
        return StmtRet(ArgumentsPtr(myarg));
    }
};

class MultiRetStmt : public Stmt {
public:
    MultiRetStmt() : Stmt(STMT_MULTI) {
    }

    virtual StmtRet execute(StmtRef prev, ArgumentsPtr args) {
        MyArguments *myarg = (MyArguments*) args.get();
        std::string s = myarg->getAsStr();
        printf("%s\n", s.c_str());
        myarg = new MyArguments();
        myarg->add("ret1: " + s);
        myarg->add("ret2: " + s);
        return StmtRet(StmtRet::CONTINUE, ArgumentsPtr(myarg));
    }
};

StmtChunkPtr createSeqStmts(StmtFactory &fac) {
    StmtChunkPtr chunk = fac.createChunk();
    StmtRef stmt = chunk->create(STMT_HELLO);
    (stmt->addPost(chunk->create(STMT_HELLO)))->
        addPost(chunk->create(STMT_HELLO))->
        addPost(chunk->create(STMT_HELLO));
    return chunk;
}

// s1 -> s2 -> s3 ...
void testSeqStmts(StmtFactory &fac) {
    auto chunk = createSeqStmts(fac);
    MyArguments *myarg = new MyArguments();
    myarg->add("kevin");
    Engine::instance().run(chunk, ArgumentsPtr(myarg), 
        [] (ChunkRet cr, ArgumentsPtr ret) -> void {
            MyArguments *myarg = (MyArguments*) ret.get();
            printf("result: %s\n", myarg->getAsStr().c_str());
    });
}

StmtChunkPtr createParallStmts(StmtFactory &fac) {
    StmtChunkPtr chunk = fac.createChunk();
    StmtRef stmt = chunk->create(STMT_MULTI);
    StmtRef spliter = chunk->create(ST_SPLITER);
    StmtRef merger = chunk->create(ST_MERGER);
    stmt->addPost(spliter);
    (spliter->addPost(chunk->create(STMT_HELLO)))->
        addPost(chunk->create(STMT_HELLO))->
        addPost(merger);
    (spliter->addPost(chunk->create(STMT_HELLO)))->
        addPost(chunk->create(STMT_HELLO))->
        addPost(merger);
    return chunk;
}

//  s1 -> spliter -> s2 -> s4 -> 
//                -> s3 -> s5 -> merger
void testParallStmts(StmtFactory &fac) { 
    auto chunk = createParallStmts(fac);
    auto stmt = chunk->head();
    MyArguments *myarg = new MyArguments();
    myarg->add("lynx");
    Engine::instance().run(chunk, ArgumentsPtr(myarg), 
        [] (ChunkRet cr, ArgumentsPtr ret) -> void {
            MyArguments *myarg = (MyArguments*) ret.get();
            printf("result: %s\n", myarg->getAsStr().c_str());
    });
}

void testEncode(StmtFactory &fac) {
    auto chunk = createParallStmts(fac);
    ByteBuffer buf;
    VectorWriteBuffer vbuf(buf);
    chunk->encode(vbuf);
    auto chunk2 = fac.createChunk();
    bool ret = chunk2->decode(VectorReadBuffer(buf));
    assert(ret);
    MyArguments *myarg = new MyArguments();
    myarg->add("lynx");
    Engine::instance().run(chunk2, ArgumentsPtr(myarg), 
        [] (ChunkRet cr, ArgumentsPtr ret) -> void {
            MyArguments *myarg = (MyArguments*) ret.get();
            printf("result: %s\n", myarg->getAsStr().c_str());
    });
}

void testView(StmtFactory &fac) {
    auto chunk = createParallStmts(fac);
    chunk->view("parallflow.dot");
    chunk = createSeqStmts(fac);
    chunk->view("seqflow.dot");
}

class NullStmt : public Stmt {
public:
    NullStmt() : Stmt(STMT_NULL) {
    }

    virtual StmtRet execute(StmtRef prev, ArgumentsPtr args) {
        auto next = new MyArguments();
        return StmtRet(ArgumentsPtr(next));
    }
};

// hello -> if -> hello -> null -> if -> hello
//             -> hello2              -> hello2
StmtChunkPtr createIfStmts(StmtFactory &fac) {
    StmtChunkPtr chunk = fac.createChunk();
    Cond *cond = new ArgNullCond();
    StmtRef stmt = chunk->create(STMT_HELLO);
    IfStmt *ifs = (IfStmt*) chunk->create(ST_IF);
    Cond *cond2 = new ArgNullCond();
    IfStmt *ifs2 = (IfStmt*) chunk->create(ST_IF);
    ifs->setCond(cond);
    ifs2->setCond(cond2);
    stmt->addPost(ifs);
    ifs->addPost(chunk->create(STMT_HELLO))->
        addPost(chunk->create(STMT_NULL))->
        addPost(ifs2);
    ifs->addPost(chunk->create(STMT_HELLO2));
    ifs2->addPost(chunk->create(STMT_HELLO));
    ifs2->addPost(chunk->create(STMT_HELLO2));
    return chunk;
}

void testIfStmts(StmtFactory &fac) {
    auto chunk = createIfStmts(fac);
    MyArguments *myarg = new MyArguments();
    myarg->add("ifstmt");
    Engine::instance().run(chunk, ArgumentsPtr(myarg), 
        [] (ChunkRet cr, ArgumentsPtr ret) -> void {
            MyArguments *myarg = (MyArguments*) ret.get();
            printf("result: %s\n", myarg->getAsStr().c_str());
    });
}

// hello -> if -> goto -> hello
//             -> inc -> goto
//           ^             |
//           +-------------+
StmtChunkPtr createLoopStmts(StmtFactory &fac) {
    StmtChunkPtr chunk = fac.createChunk();
    Cond *cond = new ArgNullCond();
    StmtRef stmt = chunk->create(STMT_HELLO);
    IfStmt *ifs = (IfStmt*) chunk->create(ST_IF);
    ifs->setCond(cond);
    ifs->addPost(chunk->create(ST_GOTO))->
        addPost(chunk->create(STMT_HELLO));
    ifs->addPost(chunk->create(STMT_INC))->
        addPost(chunk->create(ST_GOTO))->
        addPost(ifs);
    return chunk;
}

void testLoopStmts(StmtFactory &fac) {
    auto chunk = createLoopStmts(fac);
    MyArguments *myarg = new MyArguments();
    myarg->add("loopstmt");
    Engine::instance().run(chunk, ArgumentsPtr(myarg), 
        [] (ChunkRet cr, ArgumentsPtr ret) -> void {
            MyArguments *myarg = (MyArguments*) ret.get();
            printf("result: %s\n", myarg->getAsStr().c_str());
    });
}

int main(int argc, char **argv) {
    StmtFactory fac;
    fac.reg(STMT_HELLO, STMT_NEW_CREATOR(HelloStmt));
    fac.reg(STMT_MULTI, STMT_NEW_CREATOR(MultiRetStmt));
    fac.reg(STMT_HELLO2, STMT_NEW_CREATOR(Hello2Stmt));
    fac.reg(STMT_NULL, STMT_NEW_CREATOR(NullStmt));
    fac.reg(STMT_INC, STMT_NEW_CREATOR(IncStmt));
    testSeqStmts(fac);
    testParallStmts(fac);
    testEncode(fac);
    testView(fac);
    testIfStmts(fac);
    testLoopStmts(fac);
    return 0;
}
