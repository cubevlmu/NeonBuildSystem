#include "SharedLib.hpp"

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

namespace nbs {

    SharedLib::SharedLib()
        : handle(nullptr)
    {
    }


    SharedLib::~SharedLib() {
        if (handle) {
            close();
        }
    }


    bool SharedLib::load(const char* path) {
#ifdef _WIN32
        handle = LoadLibraryA(path);
#else
        handle = dlopen(path, RTLD_LAZY);
#endif
        return handle;
    }


    void* SharedLib::getFunction(const char* name)
    {
        if (!handle) {
            return nullptr;
        }
#ifdef _WIN32
        return GetProcAddress((HMODULE)handle, name);
#else
        return dlsym(handle, name);
#endif
    }


    void SharedLib::close() {
        if (handle) {
#ifdef _WIN32
            FreeLibrary((HMODULE)handle);
#else
            dlclose(handle);
#endif
            handle = nullptr;
        }
    }


    void* SharedLib::getFuncFromENV(const char* name)
    {
#ifdef _WIN32
        HMODULE handle = GetModuleHandle(nullptr);
        if (!handle) {
            return nullptr;
        }
        return GetProcAddress(handle, name);
#else
        return dlsym(RTLD_DEFAULT, name);
#endif
    }


}