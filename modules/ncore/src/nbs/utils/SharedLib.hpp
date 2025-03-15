#pragma once

namespace nbs {

    class SharedLib {
    public:
        SharedLib();
        ~SharedLib();

        bool load(const char* path);
        void* getFunction(const char* name);
        void close();

        static void* getFuncFromENV(const char* name);

    private:
        void* handle;
    };
}