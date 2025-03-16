#include "ScriptMgr.hpp"

#include <sol/sol.hpp>

namespace nbs {

    ScriptMgr::ScriptMgr(NLuaCtx* mgr)
        : m_ctx(mgr)
    {

    }

    void ScriptMgr::init()
    {
        m_state.open_libraries(sol::lib::base, sol::lib::jit, sol::lib::table);
        
    }


}