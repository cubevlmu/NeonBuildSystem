#pragma once

#include "nbs/NCmdLine.hpp"
#include "nbs/base/Logger.hpp"

#include <exception>
#include <string>

namespace nbs {

    class NBSCtx 
    {
        class PrivateData;
    public:
        enum OS { Unknown, Windows, MacOS, Linux };
        struct NBSError : std::exception {
            const char* message;
            NBSError(const char* error) {
                LogError("[ERROR] ", error);
            }
            virtual char const *what() const noexcept {
                return "nbs exception";
            }
        };

    public:
        NBSCtx(int argc, char** argv);
        ~NBSCtx();

        void globalInit();
        int run();

        const char* quiryVariable(const char* key);

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
        } globDatas;

        inline bool isDataReady() const {
            return globDatas.isInit;
        }
        inline class PluginMgr* getPluginMgr() const {
            return m_mgr;
        }

        static const char* getCurrentPlatform();
        static OS getCurrentOS();
        static NBSCtx* getInstance();

    private:
        bool runScript();

    private:
        static NBSCtx* s_ctx;

        PluginMgr* m_mgr;
        NCmdLine m_cmdLine;
        PrivateData* m_priv;
    };
}