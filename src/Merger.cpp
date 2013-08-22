//
// file: Merger.cpp
// author: Kevin Lynx
//
#include <assert.h>
#include <algorithm>
#include "Merger.h"

namespace Wee {
    StmtRet Merger::execute(StmtRef prev, ArgumentsPtr args) {
        assert(args && prev);
        m_args[(unsigned) prev] = args;
        if (m_args.size() == m_prevs.size()) {
            ArgumentList::ArgArray array;
            ArgsTable t(m_args);
            std::for_each(m_prevs.begin(), m_prevs.end(), 
                [&array, t] (WeakPtr ref) -> void {
                    unsigned key = (unsigned) ref;
                    auto it = t.find(key);
                    assert(it != t.end());
                    array.push_back(it->second);
            });
            return StmtRet(StmtRet::CONTINUE, args->merge(array));
        } 
        return StmtRet(StmtRet::MERGE_WAIT, nullptr);
    } 
}
