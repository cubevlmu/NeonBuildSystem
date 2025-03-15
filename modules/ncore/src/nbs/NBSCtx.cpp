#include "NBSCtx.hpp"

#include "nbs/api/PluginMgr.hpp"
#include "nbs/base/Logger.hpp"
#include "nbs/NBSFile.hpp"
#include "nbs/utils/Timer.hpp"

#include <nbs/utils/IOUtils.hpp>

#include <cassert>
#include <filesystem>
#include <unistd.h>

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


    NBSCtx::NBSCtx(int argc, char** argv)
        : globDatas({
            .isInit = false,
            .work_dir = std::filesystem::current_path().string(),
            .nbs_work_dir = std::filesystem::current_path().string() + "/nbs",
            .bin_dir = std::filesystem::current_path().string() + "/nbs/bin",
            .obj_dir = std::filesystem::current_path().string() + "/nbs/obj",
            .globalVariables = {},
#ifdef BUILD_DEBUG
            .isInDebug = true,
            .debug_dir = std::filesystem::current_path().string() + "/nbs/dbg",
#else
            .isInDebug = false,
#endif
            })
            , m_mgr(nullptr)
    {
    }


    NBSCtx::~NBSCtx()
    {
        globDatas.isInit = false;


    }


    void NBSCtx::globalInit()
    {
        if (globDatas.isInit) {
            LogError("Globals data has already been init!");
            std::exit(-1);
        }

#ifndef BUILD_DEBUG
        if (getExecutableDirectory() == globDatas.work_dir) {
            LogError("NBS executable file mustn't in working dir!");
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

    }


    int NBSCtx::runScript()
    {
        Timer tm{ [](long ms) {
            LogInfo("Script run last for ", ms, " milisecound");
        } };

        if (!globDatas.isInit) {
            LogError("Some essential data is not prepared! HALT");
            return -1;
        }

        if (globDatas.isInDebug) {
            LogInfo("NBS is running in debug mode! It's not suit for production environment!");
        }

        if (!std::filesystem::exists(globDatas.work_dir + "/nbs.json")) {
            LogError("Build script file not found! please check nbs.json is exist or not!");
            return -1;
        }

        // try to read nbs data
        auto nbs_file_path = globDatas.work_dir + "/nbs.json";
        NBSFile file{ nbs_file_path.c_str(), this };
        if (!file.dealAll()) {
            LogError("NBS Halt with errors!");
            return -2;
        }

        return 0;
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


}