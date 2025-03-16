#include "NLuaCtx.hpp"

#include "nbs/NBSFile.hpp"
#include "nbs/api/NBSModule.hpp"
#include "nbs/base/Logger.hpp"

nbs::NBSModule* nbs_plugin_nlua_init(nbs::NBSCtx* ctx) {
    return new nbs::NLuaCtx(ctx);
}

namespace nbs {

    static PluginCmd s_cmd[] = {
        PluginCmd {
            .key = "script",
            .desc = "run lua script",
            .handler = [](NCmdLine::NCmd* cmd) -> bool {
                for(auto& tag : cmd->tags) {
                    LogInfo(tag.key, " : ", tag.value);
                }
                LogInfo("Hello world");
                return true;
            }
        }
    };

    NBSModule::ModuleData NLuaCtx::s_data = {
        .name = "scripting",
        .author = "flybird",
        .configPrefix = "scripts",
        .version = 1001,
        .cmdMap = s_cmd,
        .cmdMapSize = 1
    };


    NLuaCtx::NLuaCtx(NBSCtx* ctx)
        : NBSModule(ctx)
        , m_mgr(this)
    {

    }


    void NLuaCtx::onInit()
    {
        LogDebug("NLua loaded!");
        
        m_mgr.init();
    }


    void NLuaCtx::onStop()
    {
        
    }


    void NLuaCtx::onConfig(ConfGroup* group)
    {
        for (auto item : group->getArray("lua")) {
            LogInfo(item);
        }

    }


    NBSModule::ModuleData NLuaCtx::getData()
    {
        return s_data;
    }

}