#include "NCmdLine.hpp"

#include "nbs/base/Logger.hpp"

#include <cctype>
#include <sstream>

namespace nbs {

    NCmdLine::NCmdLine(int argc, char** argv)
        : m_idx{ 0 }
        , m_max{ 0 }
    {
        std::stringstream ss{};
        if (argc > 1) {
            for (auto idx = 1; idx < argc; idx++) {
                ss << argv[idx] << ' ';
            }
            m_args = ss.str();
            ss.clear();
            m_args.erase(m_args.end() - 1);
        }
        
        m_max = m_args.length();
        m_idx = 0;
    }


    NCmdLine::~NCmdLine() = default;


    NCmdLine::NCmd* NCmdLine::deal()
    {
        if (m_idx != 0)
            return nullptr;

        NCmd* root = new NCmd{
            .label = "nbs",
            .tags = {},
            .subCmds = nullptr
        };
        NCmd* current = root;

        while (m_idx < m_max) {
            skipSpace();
            char c = m_args[m_idx];

            if (c == '-') {
                if (m_idx + 1 >= m_max) {
                    LogError("Unexpected token at ", m_idx + 1, ":", m_max, " is -");
                    delete root;
                    return nullptr;
                }
                char nc = m_args[m_idx + 1];
                if (nc == '-') {
                    if (m_idx + 2 >= m_max) {
                        LogError("Unexpected token at ", m_idx + 1, ":", m_max, " is -- but no tag key!");
                        delete root;
                        return nullptr;
                    }
                    nc = m_args[m_idx + 2];
                    m_idx+=1;
                    goto checkNCTag;
                }

            checkNCTag:
                if (std::isalpha(nc)) {
                    m_idx += 1;
                    size_t start = m_idx;
                    while (m_idx < m_max && !std::isspace(m_args[m_idx]) && m_args[m_idx] != '=') {
                        m_idx++;
                    }
                    auto tag_key = m_args.substr(start, m_idx - start);
                    skipSpace();

                    if (m_args[m_idx] == '=') {
                        m_idx += 1;
                        skipSpace();
                        start = m_idx;
                        while (m_idx < m_max && !std::isspace(m_args[m_idx])) {
                            m_idx++;
                        }
                        auto tag_val = m_args.substr(start, m_idx - start);
                        current->tags.push_back({ .key = tag_key, .value = tag_val });
                        m_idx++;
                        continue;
                    }
                    else {
                        // skipSpace();
                        // c = m_args[m_idx];
                        // if (c == '-') {
                        //     current->tags.push_back({ .key = tag_key, .value = "" });
                        //     continue;
                        // }
                        // start = m_idx;
                        // while (m_idx < m_max && !std::isspace(m_args[m_idx])) {
                        //     m_idx++;
                        // }
                        // auto tag_val = m_args.substr(start, m_idx - start);
                        current->tags.push_back({ .key = tag_key, .value = "" });
                        // m_idx++;
                        continue;
                    }
                }
                else {
                    LogError("Unexpected token at ", m_idx + 1, ":", m_max, " is -- but no tag key!");
                    delete root;
                    return nullptr;
                }
            }
            else if (std::isalpha(c)) {
                auto start = m_idx;
                while (m_idx < m_max && !std::isspace(m_args[m_idx])) {
                    m_idx++;
                }
                auto sub_cmd = m_args.substr(start, m_idx - start);
                NCmd* cmd = new NCmd{
                    .label = sub_cmd,
                    .tags = {},
                    .subCmds = {}
                };
                current->subCmds = cmd;
                current = cmd;
            }
            else {
                LogError("Unexpected token at ", m_idx + 1, ":", m_max, " is ", c);
                delete root;
                return nullptr;
            }
        }

        return root;
    }


}