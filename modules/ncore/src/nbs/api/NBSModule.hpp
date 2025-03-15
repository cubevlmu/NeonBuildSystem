#pragma once

namespace nbs {

    class NBSModule
    {
    public:
        struct Cmd {
            const char* key;
            const bool(*handler)(const char* key, int argc, const char** args);
            const char* desc;
        };

        struct ModuleData {
            const char* name;
            const char* author;
            const char* configPrefix;
            const int version;
            const Cmd* cmdMap;
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