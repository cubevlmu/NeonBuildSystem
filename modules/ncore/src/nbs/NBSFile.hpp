#pragma once

#include "nbs/base/Logger.hpp"
#include "json/json.h"
#include <string>
#include <unordered_map>

namespace nbs {

    class ConfGroup
    {
        friend class NBSFile;

        const char* m_prefix;
        
        ConfGroup(const char* prefix);
    public:
        class Array {
            friend class ConfGroup;
            using ValueType = const char*;

            size_t m_size;
            std::string m_key;
            ConfGroup* m_gp;

            Array(ConfGroup* gp, const std::string& key, size_t max);
        public:
            const char* get(int idx) const;

            class Iterator {
            public:
                using iterator_category = std::forward_iterator_tag;
                using value_type = ValueType;
                using difference_type = std::ptrdiff_t;
                using pointer = ValueType*;
                using reference = ValueType&;

                explicit Iterator(const Array* ptr = nullptr, size_t idx = 0);
                reference operator*() const;
                pointer operator->() const;
                Iterator& operator++();
                Iterator operator++(int);
                bool operator==(const Iterator& other) const;
                bool operator!=(const Iterator& other) const;

            private:
                size_t m_idx = 0;
                size_t m_size;
                const Array* m_arr;
            };
            Iterator begin() const {
                return Iterator(this);
            }
            Iterator end() const {
                return Iterator(this, m_size);
            }

            static const Array Empty;
        };
    public:
        const char* getValue(const char* key, const char* defVal = "");
        const Array getArray(const char* key);

    private:
        std::unordered_map<std::string, std::string> confs;
    };


    class NBSFile
    {
    public:
        struct NBSFileError : public std::exception {
            const char* message;
            NBSFile* filePtr;

            NBSFileError(const char* msg, NBSFile* file) {
                filePtr = file;
                message = msg;
            }
            virtual ~NBSFileError() = default;
            virtual char const* what() const noexcept override {
                LogError("NBSFile error when processing, ", message);
                return "NBSFile ERROR";
            };
        };

    public:
        NBSFile(const char* path, class NBSCtx*);
        ~NBSFile();

        bool dealAll();

    private:
        void dealGroupConfig(Json::Value& v, ConfGroup* gp, const std::string& parent_key);
        std::string dealWithArgumentedValue(const std::string&);

    private:
        NBSCtx* m_ctx;
        const char* m_path;
        std::unordered_map<std::string, ConfGroup*> m_groups;
    };
}