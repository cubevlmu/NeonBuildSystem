#include "NBSFile.hpp"

#include "nbs/NBSCtx.hpp"
#include "nbs/api/NBSModule.hpp"
#include "nbs/api/PluginMgr.hpp"
#include "nbs/base/Logger.hpp"
#include "nbs/utils/JsonUtils.hpp"

#include <cstring>
#include <filesystem>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>

#include <json/json.h>
#include <sstream>
#include <string>

namespace nbs {

#define NPString(STR) STR.find('$') != std::string::npos ? dealWithArgumentedValue(STR) : STR

    NBSFile::NBSFile(const char* path, NBSCtx* ctx)
        : m_ctx(ctx)
        , m_path(path)
    {
        if (!std::filesystem::exists(path)) {
            throw NBSFileError{ "file not found", this };
        }
    }


    NBSFile::~NBSFile()
    {
    }


    bool NBSFile::dealAll()
    {
        std::ifstream fs{ m_path };
        if (!fs.is_open()) {
            LogError("Failed to open nbs config file ", m_path);
            return false;
        }

        std::stringstream nf_ss{};
        std::string nf_str{};

        while (std::getline(fs, nf_str)) {
            nf_ss << nf_str << '\n';
        }
        nf_str = nf_ss.str();
        nf_ss.clear();
        nf_ss.str("");

        auto root = parseJsonFromStr(nf_str);

        std::string nf_mis_tag{};
        if (!checkTagExist(root, nf_mis_tag, { "use", "version" })) {
            LogError("Tag missing -> ", nf_mis_tag);
            return false;
        }

        // deal with use
        for (auto& item : root["use"]) {
            if (!item.isString()) {
                LogWarn("Unsupport item in use block -> ", item.type());
                continue;
            }
            auto str = NPString(item.asString());
            auto* plug = m_ctx->getPluginMgr()->getPlugin(str);
            if (plug == nullptr) {
                LogWarn("Plugin ", str, " load failed. NBS will skip it's configs");
                continue;
            }

            auto* nf_px = plug->getData().configPrefix;
            if (strlen(nf_px) == 0) {
                LogWarn("Current plugin's config prefix is not defined! Skip all.");
                continue;
            }

            auto* nf_gp = new ConfGroup{ nf_px };

            if (!root.isMember(nf_px)) {
                LogDebug("Plugin's config group not found -> ", nf_px);
                m_groups.insert({ nf_px, nf_gp });
                plug->onConfig(nf_gp);
                continue;
            }

            auto& nf_cfg_gp = root[nf_px];
            dealGroupConfig(nf_cfg_gp, nf_gp, nf_px);

            m_groups.insert({ nf_px, nf_gp });
            plug->onConfig(nf_gp);
        }

        return true;
    }


    void NBSFile::dealGroupConfig(Json::Value& v, ConfGroup* gp, const std::string& parent_key)
    {
        std::string cur_prefix = parent_key;
        cur_prefix.append(".");

        if (v.isArray()) {
            auto idx = 0;
            for (auto& item : v) {
                switch (item.type())
                {
                case Json::nullValue:
                    gp->confs.insert({ cur_prefix + std::to_string(idx), "" });
                    break;
                case Json::intValue:
                    gp->confs.insert({ cur_prefix + std::to_string(idx), std::to_string(item.asLargestInt()) });
                    break;
                case Json::uintValue:
                    gp->confs.insert({ cur_prefix + std::to_string(idx), std::to_string(item.asLargestUInt()) });
                    break;
                case Json::realValue:
                    gp->confs.insert({ cur_prefix + std::to_string(idx), std::to_string(item.asDouble()) });
                    break;
                case Json::stringValue:
                    gp->confs.insert({ cur_prefix + std::to_string(idx), item.asString() });
                    break;
                case Json::booleanValue:
                    gp->confs.insert({ cur_prefix + std::to_string(idx), std::to_string(item.asBool()) });
                    break;
                case Json::arrayValue:
                    gp->confs.insert({ cur_prefix + std::to_string(idx), std::to_string(item.size()) });
                    dealGroupConfig(item, gp, cur_prefix + std::to_string(idx));
                    break;
                case Json::objectValue:
                    dealGroupConfig(item, gp, cur_prefix + std::to_string(idx));
                    break;
                }
                idx++;
            }
            return;
        }

        for (auto& ikey : v.getMemberNames()) {
            auto item = v[ikey];
            switch (item.type())
            {
            case Json::nullValue:
                gp->confs.insert({ cur_prefix + ikey, "" });
                break;
            case Json::intValue:
                gp->confs.insert({ cur_prefix + ikey, std::to_string(item.asLargestInt()) });
                break;
            case Json::uintValue:
                gp->confs.insert({ cur_prefix + ikey, std::to_string(item.asLargestUInt()) });
                break;
            case Json::realValue:
                gp->confs.insert({ cur_prefix + ikey, std::to_string(item.asDouble()) });
                break;
            case Json::stringValue:
                gp->confs.insert({ cur_prefix + ikey, item.asString() });
                break;
            case Json::booleanValue:
                gp->confs.insert({ cur_prefix + ikey, std::to_string(item.asBool()) });
                break;
            case Json::arrayValue:
                gp->confs.insert({ cur_prefix + ikey, std::to_string(item.size()) });
                dealGroupConfig(item, gp, cur_prefix + ikey);
                break;
            case Json::objectValue:
                dealGroupConfig(item, gp, cur_prefix + ikey);
                break;
            }
        }

        return;
    }


    std::string NBSFile::dealWithArgumentedValue(const std::string&)
    {
        return std::string();
    }


    ///////////////////////////////////////////////////////////////
    //// ConfGroup::Array
    ///////////////////////////////////////////////////////////////
    ConfGroup::Array::Array(ConfGroup* gp, const std::string& key, size_t max) 
        : m_size { max }
        , m_gp { gp }
        , m_key { key }
    {
    }
    ConfGroup::Array::Iterator::Iterator(const Array* ptr, size_t idx)
        : m_arr{ ptr }
        , m_idx{ idx }
    {
    }
    ConfGroup::Array::Iterator::reference ConfGroup::Array::Iterator::operator*() const
    {
        auto val = m_arr->get(m_idx);
        return std::ref(val);
    }
    ConfGroup::Array::Iterator::pointer ConfGroup::Array::Iterator::operator->() const
    {
        auto val = m_arr->get(m_idx);
        return std::move(&val);
    }
    ConfGroup::Array::Iterator& ConfGroup::Array::Iterator::operator++()
    {
        m_idx++;
        return *this;
    }
    ConfGroup::Array::Iterator ConfGroup::Array::Iterator::operator++(int)
    {
        Iterator tmp = *this;
        ++(*this);
        return tmp;
    }
    bool ConfGroup::Array::Iterator::operator==(const Iterator& other) const
    {
        return m_idx == other.m_idx;
    }
    bool ConfGroup::Array::Iterator::operator!=(const Iterator& other) const
    {
        return m_idx != other.m_idx;
    }


    const char* ConfGroup::Array::get(int idx) const
    {
        if (idx > m_size)
            return "";

        std::string fk {m_key};
        fk.append(".").append(std::to_string(idx));

        //LogDebug("[DEBUG] Try to get array item : ", (const char*)fk.c_str());

        return m_gp->getValue(fk.c_str());
    }


    const ConfGroup::Array ConfGroup::Array::Empty{ nullptr,"",0 };


    ConfGroup::ConfGroup(const char* prefix) 
        : m_prefix { prefix }
    {}
    const char* ConfGroup::getValue(const char* key, const char* defVal)
    {
        if (strlen(key) == 0) {
            return "";
        }

        std::string fk {m_prefix};
        fk.append(".").append(key);

        //LogDebug("[DEBUG] Fullkey ", (const char*)full_key);

        if (confs.find(fk) == confs.end()) {
            return "";
        }
        return confs[fk].c_str();
    }


    const ConfGroup::Array ConfGroup::getArray(const char* key)
    {
        if (strlen(key) == 0) {
            return Array::Empty;
        }

        std::string fk {m_prefix};
        fk.append(".").append(key);

        if (confs.find(fk) == confs.end()) {
            return Array::Empty;
        }
        try {
            auto size = std::stoi(confs[fk]);
            Array arr{ this, key, (size_t)size };
            return std::move(arr);
        }
        catch (...) {
            LogDebug("Conf ", (const char*)fk.c_str(), " value is not a size value -> ", confs[key]);
            return Array::Empty;
        }
    }

}