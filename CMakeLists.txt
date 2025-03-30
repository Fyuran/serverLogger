cmake_minimum_required (VERSION 3.25.1)

project ("btc_serverLogger_x64"
    LANGUAGES CXX
)
include(FetchContent)

# Set output names based on platform
if(WIN32)
    set(OUTPUT_NAME "btc_serverLogger_x64") # Keep original name for DLL base
else()
    set(OUTPUT_NAME "btc_serverLogger_x64") # Set base name for SO
endif()

add_library (btc_serverLogger_x64 SHARED)

# Set platform-specific properties like prefix and suffix
if(WIN32)
    set_target_properties(btc_serverLogger_x64 PROPERTIES
        OUTPUT_NAME ${OUTPUT_NAME} # Uses base name
        PREFIX ""
        SUFFIX ".dll"
    )
else()
     # On Linux, ensure the 'lib' prefix is added unless CMAKE_SHARED_LIBRARY_PREFIX is changed elsewhere
    set_target_properties(btc_serverLogger_x64 PROPERTIES
        OUTPUT_NAME ${OUTPUT_NAME} # Uses base name
        PREFIX "lib" # Standard Linux prefix
        SUFFIX ".so"
    )
endif()


target_include_directories(btc_serverLogger_x64 PRIVATE
    "include"
)

# Base sources common to all platforms
target_sources (btc_serverLogger_x64 PRIVATE
    "src/dllmain.cpp"
    "src/armaLogData.cpp"
    # Do NOT list headers (.hpp) as sources here
)

# Windows-specific sources and settings
if(WIN32)
    target_sources(btc_serverLogger_x64 PRIVATE
        "src/dllmain.def" # Only needed for Windows DLL exports
        "include/coutToDebug.hpp" # Include header only if needed for compilation context (rarely)
                                  # Typically, just including it in dllmain.cpp guarded by #ifdef is enough
    )
endif()


# Set C++17 for all platforms
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Fetch and configure JSON library
FetchContent_Declare(json URL https://github.com/nlohmann/json/archive/refs/tags/v3.11.3.zip)
FetchContent_MakeAvailable(json)

# Configure Boost
# These static settings might increase binary size but reduce runtime dependencies, keep if intentional
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_STATIC_RUNTIME ON) # This influences MSVC runtime, less impact on GCC/Clang

if(MSVC)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS OFF) # Good practice when using .def file

    # Configure Boost for Windows
    if(DEFINED ENV{BOOST_ROOT})
        set(BOOST_ROOT $ENV{BOOST_ROOT})
        # Boost headers are usually under BOOST_ROOT/include, FindBoost handles this
    else()
         message(WARNING "BOOST_ROOT environment variable not set. FindBoost might fail or find system Boost.")
    endif()
    # Consider removing NO_SYSTEM_PATHS if you want FindBoost to search default locations as fallback
    # set(Boost_NO_SYSTEM_PATHS ON) # Only look in BOOST_ROOT if defined

    # MSVC-specific settings
    target_compile_options(btc_serverLogger_x64 PRIVATE
        $<$<CONFIG:Debug>:/MTd> # Link static debug runtime
        $<$<CONFIG:Release>:/MT> # Link static release runtime
        /W4
        /EHsc
        /std:c++17
        $<$<CONFIG:Release>:/O2>
    )
    # Add include directory specifically for coutToDebug if needed (usually handled by target_include_directories)
    # target_include_directories(btc_serverLogger_x64 PRIVATE include) # Already done above

elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # GCC/Clang specific options
    target_compile_options(btc_serverLogger_x64 PRIVATE
        -Wall
        -Wextra
        $<$<CONFIG:Release>:-O3>
        -D_GLIBCXX_USE_CXX11_ABI=0 # Keep if Arma Linux server requires old ABI
        -fPIC # Required for shared libraries
        -std=c++17
    )
    if(UNIX AND NOT APPLE)
        # Linker options for static standard libraries and stripping
        # Using target_link_options is generally preferred over LINK_FLAGS property
        target_link_options(btc_serverLogger_x64 PRIVATE
            -static-libgcc
            -static-libstdc++
            # -Wl,--no-as-needed # Keep if necessary, but can sometimes cause issues
            $<$<CONFIG:Release>:-Wl,--strip-all> # Strip only in release builds
        )
        # Ensure all symbols are defined (good practice for .so)
        target_link_options(btc_serverLogger_x64 PRIVATE "-Wl,--no-undefined")
    endif()
endif()

# Find Boost package (version 1.74 required, matches Debian 12)
find_package(Boost 1.74 REQUIRED COMPONENTS system filesystem thread) # Explicitly list components if known

# Link libraries
target_link_libraries(btc_serverLogger_x64 PRIVATE
    Boost::boost            # Meta-target, links components found
    Boost::filesystem       # Link explicitly if needed (esp. if not using Boost::boost)
    Boost::system           # Often a dependency of filesystem or thread
    Boost::thread           # Needed if using Boost threads
    nlohmann_json::nlohmann_json
)

# Ensure target_link_libraries uses the Boost:: components syntax which is preferred

# The LINK_FLAGS property is less preferred than target_link_options
# if(UNIX AND NOT APPLE)
#     set_target_properties(btc_serverLogger_x64 PROPERTIES
#         LINK_FLAGS "-Wl,--no-undefined" # Moved to target_link_options
#     )
# endif()

# Include glibc_version.h (though its effect is limited)
if(UNIX AND NOT APPLE)
  target_compile_definitions(btc_serverLogger_x64 PRIVATE USING_GLIBC_VERSION_H) # Example definition
  target_include_directories(btc_serverLogger_x64 PRIVATE include) # Ensure include path is set
endif()
