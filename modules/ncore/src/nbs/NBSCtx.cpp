#include "NBSCtx.hpp"

#include "nbs/api/NBSModule.hpp"
#include "nbs/api/PluginMgr.hpp"
#include "nbs/base/Logger.hpp"
#include "nbs/NBSFile.hpp"
#include "nbs/utils/Timer.hpp"

#include <nbs/utils/IOUtils.hpp>

#include <cassert>
#include <filesystem>
#include <unistd.h>
#include <unordered_map>

namespace nbs {

    static inline bool checkSetupDir(const char* path) {
        if (std::filesystem::exists(path)) {
            if (!std::filesystem::is_directory(path)) {
                LogError("Failed to init directory -> ", path, " already exist but not directory");
                return false;
            }
            return true;
        }
        return std::filesystem::create_directory(path);
    }

    class NBSCtx::PrivateData {
    public:
        std::unordered_map<std::string, std::string> globalVariables {};
        
        std::unordered_map<std::string, NCmdLine::NCmd*> subCmds;
        std::unordered_map<std::string, std::string> tags;
    };


    NBSCtx::NBSCtx(int argc, char** argv)
        : globDatas({
            .isInit = false,
#ifdef BUILD_DEBUG
            .isInDebug = true,
            .debug_dir = std::filesystem::current_path().string() + "/nbs/dbg",
#else
            .isInDebug = false,
#endif
            .nbs_work_dir = std::filesystem::current_path().string() + "/nbs",
            .work_dir = std::filesystem::current_path().string(),
            .bin_dir = std::filesystem::current_path().string() + "/nbs/bin",
            .obj_dir = std::filesystem::current_path().string() + "/nbs/obj"
            })
        , m_mgr(nullptr)
        , m_cmdLine(argc, argv)
        , m_priv{ new PrivateData {} }
    {
        s_ctx = this;
    }


    NBSCtx::~NBSCtx()
    {
        globDatas.isInit = false;

        // do some logic for releasing 

        if (m_mgr)
            delete m_mgr;
        delete m_priv;
    }


    void NBSCtx::globalInit()
    {
        if (globDatas.isInit) {
            LogError("Globals data has already been init!");
            std::exit(-1);
        }

#ifndef BUILD_DEBUG
        if (getExecutableDirectory() == globDatas.work_dir) {
            LogError("NBS executable file mustn't in the working dir!");
            std::exit(-1);
        }
#endif

        if (!checkSetupDir(globDatas.nbs_work_dir.c_str()) || !checkSetupDir(globDatas.bin_dir.c_str()) || !checkSetupDir(globDatas.obj_dir.c_str())) {
            LogError("Failed to init directories for nbs!");
            std::exit(-1);
        }
#ifdef BUILD_DEBUG
        assert(checkSetupDir(globDatas.debug_dir.c_str()));
#endif

        globDatas.isInit = true;

        // do some init logic
        m_mgr = new PluginMgr(this);

        // init global variables
        m_priv->globalVariables.insert({"nbs_work_dir", globDatas.work_dir});
        m_priv->globalVariables.insert({"nbs_data_dir", globDatas.nbs_work_dir});
        m_priv->globalVariables.insert({"nbs_bin_dir", globDatas.bin_dir});
        m_priv->globalVariables.insert({"nbs_obj_dir", globDatas.obj_dir});

        // deal with cmd lines
        auto* cmd = m_cmdLine.deal();
        if (cmd == nullptr) {
            LogError("Failed to parse commandline arguments!");
            std::exit(-1);
        }
        if (cmd->label != "nbs") {
            LogError("Unexpected token -> ", cmd->label, " should be nbs");
            std::exit(-1);
        }
        for (auto& item : cmd->tags) {
            m_priv->tags.insert({item.key, item.value});
        }
        
        checkSubCmd:
        auto* subCmd = cmd->subCmds;
        if (subCmd == nullptr)
            goto endSubCmd;
        m_priv->subCmds.insert({subCmd->label, subCmd});
        subCmd = subCmd->subCmds;

        endSubCmd:
        return;
    }


    int NBSCtx::run()
    {
        Timer tm{ [](long ms) {
            LogInfo("Task done in ", ((double)ms / 1000), " s");
        } };

        if (!globDatas.isInit) {
            LogError("Some essential data is not prepared! HALT");
            return -1;
        }

        if (globDatas.isInDebug) {
            LogWarn("NBS is running in debug mode! It's not suit for production environment!");
        }

        if (m_priv->subCmds.size() == 0 || m_priv->subCmds.find("help") != m_priv->subCmds.end() || m_priv->tags.find("help") != m_priv->tags.end()) {
            printf("NeonBuildSystem:\n\n");
            printf("   help       :  print the help text\n");
            printf("   setup      :  run the nbs script to setup project\n");
            printf("   <subcmd>   :  run plugin's command\n");
            printf("   clean      :  clean up the cache and datas\n");
            printf("   build      :  run the build task of current project\n");
            printf("\n");
            std::exit(0);
        }

        if (m_priv->subCmds.size() == 0) {
            return 0;
        }
        auto subCmdKey = m_priv->subCmds.begin()->first;
        if (subCmdKey == "setup") {
            bool r = runScript();
            return r ? 0 : -1;
        } else if (subCmdKey == "build") {
            //TODO build cmd
            return 0;
        } else if (subCmdKey == "clean") {
            //TODO clean cmd
            return 0;
        } else {
            auto* cmd = m_mgr->checkCmd(subCmdKey.c_str());
            if (cmd == nullptr) {
                LogError("Unknown command -> ", subCmdKey);
                return -1;
            }
            auto* real_cmd = m_priv->subCmds.begin()->second;
            auto r = cmd->handler(real_cmd);
            if (!r) {
                LogError("Failed to run command ", subCmdKey);
                return -1;
            }

            return 0;
        }

        return 0;
    }


    const char* NBSCtx::quiryVariable(const char* key)
    {
        if (!globDatas.isInit) {
            LogError("Global data is not init!");
            return "";
        }

        char real[strlen(key)+1];
        for (int i = 0; i < strlen(key); i++)
            real[i] = tolower(key[i]);
        real[strlen(key)] = '\0';
        
        if (m_priv->globalVariables.find((const char*)real) == m_priv->globalVariables.end()) {
            return "";
        }
        return m_priv->globalVariables[(const char*)real].c_str();
    }

    // , "$[NBS_ROOT]/plugins/npkg"

    const char* NBSCtx::getCurrentPlatform()
    {
#ifdef __WIN32
        return "windows";
#elif __MACH__
        return "macos";
#elif __LINUX__
        return "linux";
#else
        return "unknown";
#endif
    }


    NBSCtx::OS NBSCtx::getCurrentOS()
    {
#ifdef __WIN32
        return OS::Windows;
#elif __MACH__
        return OS::MacOS;
#elif __LINUX__
        return OS::Linux;
#else
        return OS::Unknown;
#endif
    }

    NBSCtx* NBSCtx::s_ctx = nullptr;
    NBSCtx* NBSCtx::getInstance()
    {
        if (s_ctx == nullptr) {
            throw NBSError{ "some component trying to get context instance before globalInit!" };
        }
        return s_ctx;
    }


    bool NBSCtx::runScript()
    {
        if (!std::filesystem::exists(globDatas.work_dir + "/nbs.json")) {
            LogError("Build script file not found! please check nbs.json is exist or not!");
            return -1;
        }

        // try to read nbs data
        auto nbs_file_path = globDatas.work_dir + "/nbs.json";
        NBSFile file{ nbs_file_path.c_str(), this };
        if (!file.dealAll()) {
            LogError("Task end with errors!");
            return false;
        }

        return true;
    }


}