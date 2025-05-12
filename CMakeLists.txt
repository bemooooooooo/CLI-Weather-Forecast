cmake_minimum_required(VERSION 3.12)

project(
    labwork7
    VERSION 1.0
    DESCRIPTION "C++ Year 2023 labwork7"
    LANGUAGES CXX
)

include(FetchContent)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
                         GIT_TAG 3b15fa82ea74739b574d705fea44959b58142eb8)
FetchContent_MakeAvailable(cpr)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_COMPILER "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/Llvm/x64/bin/clang-cl.exe")
set(CMAKE_C_COMPILER_ID "Clang")
set(CMAKE_C_COMPILER_FRONTEND_VARIANT "MSVC")
set(CMAKE_CXX_COMPILER "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/Llvm/x64/bin/clang-cl.exe")
set(CMAKE_CXX_COMPILER_ID "Clang")
set(CMAKE_CXX_COMPILER_FRONTEND_VARIANT "MSVC")

add_subdirectory(lib)
add_subdirectory(bin)
