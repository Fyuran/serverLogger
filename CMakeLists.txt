cmake_minimum_required (VERSION 3.25.1)

project ("btc_serverLogger_x64"
    LANGUAGES CXX
)
include(FetchContent)

# Set output names based on platform
if(WIN32)
    set(OUTPUT_NAME "btc_serverLogger_x64.dll")
else()
    set(OUTPUT_NAME "libbtc_serverLogger_x64.so")
endif()

add_library (btc_serverLogger_x64 SHARED)
set_target_properties(btc_serverLogger_x64 PROPERTIES 
    OUTPUT_NAME "btc_serverLogger_x64"
    PREFIX ""
    SUFFIX ${CMAKE_SHARED_LIBRARY_SUFFIX}
)

target_include_directories(btc_serverLogger_x64 PRIVATE
    "include"
)

target_sources (btc_serverLogger_x64 PRIVATE 
    "src/dllmain.cpp"
    "src/dllmain.def"
    "src/armaLogData.cpp"
    "include/armaLogData.hpp"
    "include/coutToDebug.hpp"
    "include/dllmain.hpp"
)

# Set C++17 for all platforms
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Fetch and configure JSON library
FetchContent_Declare(json URL https://github.com/nlohmann/json/archive/refs/tags/v3.11.3.zip)
FetchContent_MakeAvailable(json)

# Configure Boost
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_STATIC_RUNTIME ON)

if(MSVC)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS OFF)
    
    # Configure Boost for Windows
    if(DEFINED ENV{BOOST_ROOT})
        set(BOOST_ROOT $ENV{BOOST_ROOT})
        set(BOOST_INCLUDEDIR "${BOOST_ROOT}/include")
    endif()
    set(Boost_NO_SYSTEM_PATHS ON)

    # MSVC-specific settings
    target_compile_options(btc_serverLogger_x64 PRIVATE
        $<$<CONFIG:>:/MT>
        $<$<CONFIG:Debug>:/MTd>
        $<$<CONFIG:Release>:/MT>
        /W4
        /EHsc
        /std:c++17
        $<$<CONFIG:Release>:/O2>
    )
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # GCC-specific options
    target_compile_options(btc_serverLogger_x64 PRIVATE
        -Wall
        -Wextra
        $<$<CONFIG:Release>:-O3>
        -D_GLIBCXX_USE_CXX11_ABI=0
        -fPIC
        -std=c++17
    )
    if(UNIX AND NOT APPLE)
        target_link_options(btc_serverLogger_x64 PRIVATE
            -static-libgcc
            -static-libstdc++
            -Wl,--no-as-needed
            -Wl,--strip-all
        )
    endif()
endif()

# Find Boost package
find_package(Boost 1.74 REQUIRED)

target_link_libraries(btc_serverLogger_x64 PRIVATE 
    Boost::boost 
    nlohmann_json::nlohmann_json
)

# Additional static linking configurations for Linux
if(UNIX AND NOT APPLE)
    set_target_properties(btc_serverLogger_x64 PROPERTIES
        LINK_FLAGS "-Wl,--no-undefined"
    )
endif()
