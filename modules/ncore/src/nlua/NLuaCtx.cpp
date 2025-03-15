#include "NLuaCtx.hpp"

#include "nbs/NBSFile.hpp"
#include "nbs/base/Logger.hpp"

nbs::NBSModule* nbs_plugin_nlua_init(nbs::NBSCtx* ctx) {
    return new nbs::NLua(ctx);
}

namespace nbs {

    NBSModule::ModuleData NLua::s_data = {
        .name = "scripting",
        .author = "flybird",
        .configPrefix = "scripts",
        .version = 1001
    };


    NLua::NLua(NBSCtx* ctx)
        : NBSModule(ctx)
    {

    }


    void NLua::onInit()
    {
        LogDebug("NLua loaded!");
    }


    void NLua::onStop()
    {
    }


    void NLua::onConfig(ConfGroup* group)
    {
        for(auto item : group->getArray("lua")) {
            LogDebug("CFG: ", item);
        }

    }


    NBSModule::ModuleData NLua::getData()
    {
        return s_data;
    }

}