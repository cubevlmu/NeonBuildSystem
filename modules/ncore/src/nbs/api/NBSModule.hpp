#pragma once

#include "nbs/NCmdLine.hpp"

namespace nbs {

    struct PluginCmd {
        typedef bool(*flag)(int, char*&, char*&);
        const char* key;
        const char* desc;
        bool(*handler)(NCmdLine::NCmd*);
    };


    class NBSModule
    {
    public:
        struct ModuleData {
            const char* name;
            const char* author;
            const char* configPrefix;
            const int version;
            PluginCmd* cmdMap;
            long cmdMapSize;
        };

    public:
        NBSModule(class NBSCtx*);
        virtual ~NBSModule() = default;

        virtual void onInit() = 0;
        virtual void onStop() = 0;
        virtual void onConfig(class ConfGroup*) = 0;

        virtual ModuleData getData() = 0;

    protected:
        NBSCtx* m_ctx;
    };
}