#include "PluginMgr.hpp"

#include "nbs/NBSCtx.hpp"
#include "nbs/base/Logger.hpp"
#include "nbs/utils/SharedLib.hpp"
#include "nbs/utils/JsonUtils.hpp"
#include "nbs/utils/StringUtils.hpp"
#include "NBSModule.hpp"

#include <filesystem>
#include <sstream>
#include <fstream>
#include <json/json.h>

#define DEFAULT_PREFIX_PLUGIN "nbs_plugin_"
#define NBS_PLUG_CONF_FILE "nbs_plug.json"
#define NBS_PLUG_SIGN_FILE "nbs_plug.sign"

namespace nbs {

    PluginMgr::PluginMgr(NBSCtx* ctx)
        : m_ctx(ctx)
        , m_pluginLoaded()
    {
        if (!ctx->isDataReady()) {
            throw PluginMgrErr("PluginManager should init after NBSContext!", ctx);
        }

        scanInternals();
    }


    PluginMgr::~PluginMgr()
    {
        releaseAll();
    }


    NBSModule* PluginMgr::loadPlugin(const std::string& id)
    {
        if (id.empty()) {
            LogError("[Plugin] Plugin id mustn't be null or empty!");
            return nullptr;
        }

        if (id.starts_with('(') && id.ends_with(')')) {
            std::string new_id{ id };
            new_id.erase(new_id.begin());
            new_id.erase(new_id.end() - 1);
            return loadPluginFromDir(new_id);
        }

        if (m_pluginLoaded.find(id) != m_pluginLoaded.end()) {
            return m_pluginLoaded[id]->module;
        }

        std::string func_id{};
        func_id.append("nbs_plugin_").append(id).append("_init");

        typedef NBSModule* (func_init_sym)(NBSCtx*);
        auto* func_ptr = (func_init_sym*)SharedLib::getFuncFromENV(func_id.c_str());
        if (func_ptr == nullptr) {
            LogError("[Plugin] Plugin is not found or not valid -> id : ", id);
            return nullptr;
        }

        auto* plug_ins = func_ptr(m_ctx);
        if (plug_ins == nullptr) {
            LogError("[Plugin] Failed to load plugin -> id : ", id);
            return nullptr;
        }
        plug_ins->onInit();

        auto data = plug_ins->getData();
        LogDebug("[Plugin] Loaded! ", data.name, " by ", data.author);
        ManagedPlugin* plug_man = new ManagedPlugin{
            .mgr = this,
            .module = plug_ins,
            .ref = {0}
        };
        m_pluginLoaded.insert({ id, plug_man });

        return plug_ins;
    }


    NBSModule* PluginMgr::loadPluginFromDir(const std::string& path)
    {
        if (path.empty()) {
            LogError("[Plugin] Plugin path mustn't be null or empty!");
            return nullptr;
        }

        std::filesystem::path plug_path{ path };
        std::string plug_path_raw = plug_path.filename().string();
        if (!std::filesystem::exists(plug_path_raw) || !std::filesystem::is_directory(plug_path_raw)) {
            LogError("[Plugin] No NBS plugin in -> ", path, " [full at -> ", plug_path_raw, "]");
            return nullptr;
        }

        const char* tod_file_name = "";
        u32 tod_idx = 0;
    checkExist:
        switch (tod_idx) {
        case 0: tod_file_name = NBS_PLUG_CONF_FILE; break;
        case 1: tod_file_name = NBS_PLUG_SIGN_FILE; break;
        default: tod_file_name = ""; break;
        }
        if (strlen(tod_file_name) == 0)
            goto checkEnd;
        if (!std::filesystem::exists(plug_path_raw + "/" + tod_file_name)) {
            LogError("[Plugin] Invalid plugin! File not found in plugin dir -> ", tod_file_name);
            return nullptr;
        }
        tod_idx++;
        goto checkExist;

    checkEnd:
        // try to load nbs_plugin.json
        std::string plug_mnf_path = plug_path_raw + "/" + NBS_PLUG_CONF_FILE;
        std::stringstream plug_mnf_ss{};
        std::string plug_mnf{};
        std::ifstream plug_mnf_stream{};

        plug_mnf_stream.open(plug_mnf_path);
        if (!plug_mnf_stream.is_open()) {
            LogError("[Plugin] Failed to open manifest file of plugin!");
            return nullptr;
        }

        while (std::getline(plug_mnf_stream, plug_mnf)) {
            plug_mnf_ss << plug_mnf << '\n';
        }
        plug_mnf = "";
        plug_mnf = plug_mnf_ss.str();
        plug_mnf_ss.clear();
        plug_mnf_ss.str("");

        plug_mnf_stream.close();

        // try to parse nbs_plugin.json
        Json::Value mnf_root = parseJsonFromStr(plug_mnf.c_str());

        std::string mnf_check_err {};
        if (!checkTagExist(mnf_root, mnf_check_err, {"package", "libName", "supportOS", "loadBefore"})) {
            LogError("[Plugin] Invalid manifest! Key not found -> ", mnf_check_err);
            return nullptr;
        }

        // try to read config for supported platform and libName
        bool plat_allow = false;
        for (auto& os_val : mnf_root["supportOS"]) {
            if (strcmp(os_val.asCString(), NBSCtx::getCurrentPlatform())) {
                plat_allow = true;
                break;
            }
        }

        if (!plat_allow) {
            LogError("[Plugin] Plugin doesn't support current platform!");
            return nullptr;
        }

        auto libName = mnf_root["libName"].asString();
        switch (NBSCtx::getCurrentOS()) {
        case NBSCtx::Unknown:
            LogError("[Plugin] Unknown error occur!");
            return nullptr;
        case NBSCtx::Windows:
            libName.append(".dll");
            break;
        case NBSCtx::MacOS:
            libName.append(".dylib");
            break;
        case NBSCtx::Linux:
            libName.append(".so");
            break;
        }
        libName = plug_path_raw + "/" + libName;

        if (!std::filesystem::exists(libName)) {
            LogError("[Plugin] Plugin body not found! -> ", libName);
            return nullptr;
        }

        std::string id = mnf_root["package"].asString();
        replaceAll(id, ".", "_");

        if (m_pluginLoaded.find(id) != m_pluginLoaded.end()) {
            return m_pluginLoaded[id]->module;
        }

        // deal with load before
        std::vector<std::string> plug_dep_new{};
        for (auto& item : mnf_root["loadBefore"]) {
            auto id = item.asCString();
            if (m_pluginLoaded.find(id) != m_pluginLoaded.end()) {
                m_pluginLoaded[id]->ref.fetch_add(1);
                LogDebug("[Plugin] Mark refed to ", id);
                continue;
            }
            LogDebug("[Plugin] Trying to load dependency plugin -> ", id);
            plug_dep_new.push_back(id);
            loadPlugin(id);

            LogDebug("[Plugin] Mark refed to ", id);
            m_pluginLoaded[id]->ref.fetch_add(1);
        }


        SharedLib lib{};
        if (!lib.load(libName.c_str())) {
            LogError("[Plugin] Failed to load plugin body!");
            releasePlugins(plug_dep_new);
            return nullptr;
        }

        std::string func_id{};
        func_id.append("nbs_plugin_").append(id).append("_init");

        typedef NBSModule* (func_init_sym)(NBSCtx*);
        auto* func_ptr = (func_init_sym*)SharedLib::getFuncFromENV(func_id.c_str());
        if (func_ptr == nullptr) {
            LogError("[Plugin] Plugin is not found or not valid -> id : ", id);
            releasePlugins(plug_dep_new);
            return nullptr;
        }

        auto* plug_ins = func_ptr(m_ctx);
        if (plug_ins == nullptr) {
            LogError("[Plugin] Failed to load plugin -> id : ", id);
            releasePlugins(plug_dep_new);
            return nullptr;
        }
        plug_ins->onInit();

        auto data = plug_ins->getData();
        LogDebug("[Plugin] Loaded! ", data.name, " by ", data.author);
        ManagedPlugin* plug_man = new ManagedPlugin{
            .mgr = this,
            .module = plug_ins,
            .ref = {0}
        };
        m_pluginLoaded.insert({ id, plug_man });

        return plug_ins;
    }


    void PluginMgr::releasePlugin(const std::string& id)
    {
        if (m_pluginLoaded.find(id) == m_pluginLoaded.end()) {
            LogError("[Plugin] Trying to unload ", id, " but not found in loaded plugins");
            return;
        }
        
        auto* ptr = m_pluginLoaded[id];
        if (ptr->ref.load() != 0) {
            LogError("[Plugin] Plugin '", id, "' is refed by others, can't be release! RefCount -> ", ptr->ref.load());
            return;
        }

        ptr->module->onStop();
        delete ptr->module;
        delete ptr;
        m_pluginLoaded.erase(id);
        LogDebug("[Plugin] Plugin Unloaded -> ", id);
    }


    void PluginMgr::releaseAll()
    {
        m_mtx.lock();

        LogWarn("[Plugin] Trying to unload all plugins forced!");

        for (auto& [key,v] : m_pluginLoaded) {
            v->module->onStop();
        }

        for (auto& [key,v] : m_pluginLoaded) {
            delete v->module;
            delete v;
        }

        m_pluginLoaded.clear();

        m_mtx.unlock();
    }


    NBSModule* PluginMgr::getPlugin(const std::string& id)
    {
        if (m_pluginLoaded.find(id) == m_pluginLoaded.end()) {
            auto* ptr = loadPlugin(id);
            if (ptr == nullptr) {
                LogError("[Plugin] Plugin not found! -> ", id);
                throw PluginMgrErr { "Plugin not loaded!", m_ctx};
            }
            return ptr;
        }

        return m_pluginLoaded[id]->module;
    }


    void PluginMgr::markRelated(const std::string& id)
    {
    }


    void PluginMgr::markRelease(const std::string& id)
    {
    }


    void PluginMgr::releasePlugins(const std::vector<std::string>& plugins)
    {
        for (auto& id : plugins) {
            m_pluginLoaded[id]->ref.fetch_sub(1);
            LogDebug("[Plugin] Unref plugin to ", id);
            releasePlugin(id);
        }
    }


    void PluginMgr::scanInternals()
    {
        LogDebug("[Plugin] Trying to load internal plugins");
        loadPlugin("nlua");
    }


}