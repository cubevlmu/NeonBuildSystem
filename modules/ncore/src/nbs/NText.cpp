#include "NText.hpp"
#include "nbs/NBSCtx.hpp"

#include <sstream>

namespace nbs {

#define SKIP_SPACE { while(m_idx < m_max) { if (!std::isspace(txt[m_idx])) break; m_idx++; } }

    NText::NText(const std::string_view& txt)
        : m_idx {0}
        , m_max {(u64)txt.length()}
        , m_result {""}
    {
        size_t placeholderPos = txt.find("$[");
        if (placeholderPos == std::string::npos) {
            m_idx = 0;
            m_max = 0;
            m_result = std::move(txt);
            return;
        }

        std::stringstream ss {};
        while (m_idx < m_max) {
            SKIP_SPACE;
            char c = txt[m_idx];

            if (c == '$' && m_idx + 1 < m_max) {
                char cn = txt[m_idx + 1];
                if (cn != '[')
                   goto normal;
                size_t start = m_idx;
                while (m_idx < m_max && !std::isspace(txt[m_idx]) && txt[m_idx] != ']')
                    m_idx++;
                if (std::isspace(txt[m_idx])) {
                    m_idx = start;
                    goto normal;
                }
                
                char dest[m_idx - start-2];
                strcpy(dest, txt.data() + start+2);
                dest[m_idx - start-2] = '\0';
                const char* key = (const char*)dest;

                auto val = NBSCtx::getInstance()->quiryVariable(key);
                if (strlen(val) == 0) {
                    ss << '$' << '[' << dest << ']';
                    goto end;
                }

                ss << val;
                m_idx++;
                continue;
            }
            
            normal:
            ss << c;
            end:
            m_idx++;
        }

        m_result = ss.str();
        ss.clear();
        ss.str("");

        m_idx = 0;
        m_max = 0;
    }


}