project(NCore)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_OBJC_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(includeList 
    "src"
)
set(linkList
    ""
)
set(dependenciesList 
    ""
)
set(headerList 
    ""
)


file(GLOB_RECURSE src
    "src/*.m"
    "src/*.cpp"
    "src/*.mm"
    "src/*.c"
)
add_executable(NCore
    ${src}
)

InstallMacOS(includeList linkList)
InstallCLI11(includeList linkList)
InstallSpdlog(includeList linkList)
InstallJsonCPP(includeList linkList)

IF(${dependenciesList})
add_dependencies(NCore 
    ${dependenciesList}
)
ENDIF()
target_include_directories(NCore PRIVATE 
    ${includeList}
)
target_link_libraries(NCore PRIVATE 
    ${linkList}
)

target_compile_definitions(NCore PRIVATE 
    STATIC_PLUGINS

    BUILD_DEBUG

    ${LM_PLATFORM}
    ${LM_POSIX_SUPPORT}
)
