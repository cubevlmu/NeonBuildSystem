#pragma once

#include <globals.hpp>

#include "nbs/NBSCtx.hpp"
#include "nbs/api/NBSModule.hpp"

extern "C" {
    nbs::NBSModule* nbs_plugin_nlua_init(nbs::NBSCtx*); 
}

namespace nbs {

    class NLua : public nbs::NBSModule 
    {
    public:
        NLua(NBSCtx* ctx);
        virtual ~NLua() = default;

        virtual void onInit() override;
        virtual void onStop() override;
        virtual void onConfig(struct ConfGroup*) override;
        virtual ModuleData getData() override;

    private:
        static NBSModule::ModuleData s_data;
    };
    
}