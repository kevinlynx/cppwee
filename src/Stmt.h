//
// file: Stmt.h
// author: Kevin Lynx
//
#ifndef CPPWEE_STMT_H
#define CPPWEE_STMT_H

#include "f_decl.h"
#include <vector>
#include <string>

class ReadBuffer;
class WriteBuffer;

namespace Wee {
    // collect all arguments passed to a stmt, or results returned from a stmt.
    class ArgumentList {
    public:
        typedef std::shared_ptr<ArgumentList> Ptr;
        typedef std::vector<Ptr> ArgArray;

    public: 
        virtual ~ArgumentList() {}

        // split this and return a list of new arguments
        virtual ArgArray Split() = 0;

        // create a new arguments which merge all args
        virtual Ptr Merge(const ArgArray &args) = 0;
    };
    typedef ArgumentList::Ptr ArgumentsPtr;

    // statement execution result
    class StmtRet {
    public:
        // `CONTINUE': the normal status, try to execute the next statement
        // `END': the whole execution flow reaches the end
        // `HANGUP': one of the execution flow stop but will restart later
        // `HALT': the whole execution flow stop but will restart later
        enum { CONTINUE, END, HANGUP, HALT, SPLIT, MERGE_WAIT };

    public:
        StmtRet(ArgumentsPtr args) : m_args(args), m_code(CONTINUE) {
        }

        StmtRet(int code, ArgumentsPtr args) : m_args(args), m_code(code) {
        }

        int code() const {
            return m_code;
        }

        ArgumentsPtr args() const {
            return m_args;
        }

    private:
        int m_code;
        ArgumentsPtr m_args;
    };

    class RunInstance;
    class Pipeline;

    typedef unsigned char StmtType;

    enum StmtTypes {
        ST_NULL = 0,
        ST_MERGER,
        ST_SPLITER,
        ST_USER = 0x0f, // user statements start here
    };

    class Stmt {
    public:
        typedef std::shared_ptr<Stmt> Ptr;
        typedef Stmt* WeakPtr;
        typedef std::vector<WeakPtr> StmtRefs;

    public:
        Stmt(StmtType type) : m_inst(NULL), m_type(type) {
        }

        virtual ~Stmt() {
        }

        // set runtime context
        void setContext(RunInstance *inst, Pipeline *pipe);

        // add a post statement to this, and this statement will be the previous statements
        WeakPtr addPost(WeakPtr stmt);

        // encode this statement
        virtual void encode(WriteBuffer &buf);

        // decode this statement
        virtual void decode(ReadBuffer &buf);

        // execute this statement
        virtual StmtRet execute(WeakPtr prev, ArgumentsPtr args) = 0;

        // output statements linkage as Graphviz format
        virtual std::string view();

        // only copy data, it's used in sub classes
        virtual StmtRef copy(StmtRef other) const;

        StmtRefs posts() const {
            return m_posts;
        }

        StmtRefs prevs() const {
            return m_prevs;
        }

        StmtType type() const {
            return m_type;
        }

    private:
        void addPrev(WeakPtr stmt);

    protected:
        StmtType m_type;
        StmtRefs m_posts;
        StmtRefs m_prevs;
        RunInstance *m_inst;
        Pipeline *m_pipe;
    };
    typedef Stmt::Ptr StmtPtr;
    typedef Stmt::WeakPtr StmtRef;
}

#endif // CPPWEE_STMT_H
