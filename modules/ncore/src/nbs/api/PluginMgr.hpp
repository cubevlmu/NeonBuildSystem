#pragma once

#include "globals.hpp"
#include <atomic>
#include <exception>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace nbs {
    
    class PluginMgr
    {
        struct ManagedPlugin {
            PluginMgr* mgr;
            class NBSModule* module;
            std::atomic<u32> ref;
            bool isInternalPlugin;
            const std::string path;
        };

    public:
        struct PluginMgrErr : public std::exception {
            const char* reason;
            class NBSCtx* current_ctx;

            PluginMgrErr(const char* reason, NBSCtx* ctx)
                : reason(reason)
                , current_ctx(ctx) 
            {}
        };

    public:
        PluginMgr(NBSCtx*);
        ~PluginMgr();

        NBSModule* loadPlugin(const std::string& id);
        NBSModule* loadPluginFromDir(const std::string& path);

        void releasePlugin(const std::string& id);
        void releaseAll();
        NBSModule* getPlugin(const std::string& id);
        struct PluginCmd* checkCmd(const char* label);
    
    private:
        void markRelated(const std::string& id);
        void markRelease(const std::string& id);
        
        void releasePlugins(const std::vector<std::string>& plugins);
        void scanInternals();

    private:
        static const u32 MAGIC = 'NBPC';

        NBSCtx* m_ctx;
        std::mutex m_mtx;
        std::unordered_map<std::string, ManagedPlugin*> m_pluginLoaded;
    };
}