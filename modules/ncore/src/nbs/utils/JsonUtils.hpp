#pragma once

#include "nbs/base/Logger.hpp"

#include <globals.hpp>
#include <initializer_list>
#include <json/json.h>

namespace nbs {

    inline static Json::Value parseJsonFromStr(const std::string &mystr)
    {
        Json::CharReaderBuilder ReaderBuilder;
        ReaderBuilder["emitUTF8"] = true; 

        Json::CharReader* charread = ReaderBuilder.newCharReader();
        Json::Value root;
        
        std::string strerr {};
        bool isok = charread->parse(mystr.c_str(), mystr.c_str() + mystr.size(), &root, &strerr);
        if (!isok || strerr.size() != 0)
        {
            LogError("[Json] Parsed error!");
        }
        delete charread;
        
        return root;
    }


    inline static bool checkTagExist(Json::Value& obj, std::string& missing_tag, const std::initializer_list<std::string> tags) {
        missing_tag = "";

        for (auto& tag : tags) {
            if (!obj.isMember(tag.c_str())) {
                missing_tag = tag;
                return false;
            }
        }

        return true;
    }
}