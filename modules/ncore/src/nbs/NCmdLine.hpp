#pragma once

#include <cctype>
#include <string>
#include <vector>

namespace nbs {

    class NCmdLine 
    {
    public:
        struct NTag {
            std::string key;
            std::string value;
        };

        struct NCmd {
            std::string label;
            std::vector<NTag> tags;
            NCmd* subCmds;

            ~NCmd() {
                if(subCmds) 
                    delete subCmds;
            }
        };
    public:
        NCmdLine(int argc, char** argv);
        ~NCmdLine();

        NCmd* deal();

    private:
        inline void skipSpace()
        {
            while(m_idx < m_max) {
                if (!std::isspace(m_args[m_idx]))
                    break;
                m_idx++;
            }
        }

    private:
        std::string m_args;
        size_t m_max;
        size_t m_idx;
    };
}