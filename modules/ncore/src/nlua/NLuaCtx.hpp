#pragma once

#include <globals.hpp>

#include "nbs/NBSCtx.hpp"
#include "nbs/api/NBSModule.hpp"

#include "ScriptMgr.hpp"

struct lua_State;

extern "C" {
    nbs::NBSModule* nbs_plugin_nlua_init(nbs::NBSCtx*); 
}

namespace nbs {

    class NLuaCtx : public nbs::NBSModule 
    {
    public:
        NLuaCtx(NBSCtx* ctx);
        virtual ~NLuaCtx() = default;

        virtual void onInit() override;
        virtual void onStop() override;
        virtual void onConfig(struct ConfGroup*) override;
        virtual ModuleData getData() override;

    private:
        static NBSModule::ModuleData s_data;
        ScriptMgr m_mgr;
    };
    
}