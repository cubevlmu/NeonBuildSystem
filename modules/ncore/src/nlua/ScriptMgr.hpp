#pragma once

#include <sol/sol.hpp>

namespace nbs {

    class ScriptMgr 
    {
    public:
        ScriptMgr(class NLuaCtx*);

        void init();

    private:
        sol::state m_state;
        NLuaCtx* m_ctx;
    };
}