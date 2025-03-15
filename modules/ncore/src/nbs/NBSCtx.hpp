#pragma once

#include <unordered_map>
#include <string>

namespace nbs {

    class NBSCtx 
    {
    public:
        enum OS { Unknown, Windows, MacOS, Linux };

    public:
        NBSCtx(int argc, char** argv);
        ~NBSCtx();

        void globalInit();
        int runScript();

        struct CtxData {
            const int version_code = '0101';
            bool isInit = false;
            const bool isInDebug;

#ifdef BUILD_DEBUG
            const std::string debug_dir;
#endif
            const std::string nbs_work_dir;
            const std::string work_dir;
            const std::string bin_dir;
            const std::string obj_dir;
            
            std::unordered_map<std::string, std::string> globalVariables;  
        } globDatas;

        inline bool isDataReady() const {
            return globDatas.isInit;
        }
        inline class PluginMgr* getPluginMgr() const {
            return m_mgr;
        }

        static const char* getCurrentPlatform();
        static OS getCurrentOS();

    private:
        PluginMgr* m_mgr;
    };
}