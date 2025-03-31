if(UNIX AND NOT APPLE)
cmake_minimum_required (VERSION 3.25)
else()
cmake_minimum_required (VERSION 3.40)
endif()

set(VCPKG_OVERLAY_TRIPLETS "${CMAKE_CURRENT_SOURCE_DIR}/out/build/triplets")
if(WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS OFF)
    set(VCPKG_TARGET_TRIPLET x64-windows-static)
    add_compile_definitions(_WIN32_WINNT=0x0A00)
elseif(UNIX AND NOT APPLE)
    set(VCPKG_TARGET_TRIPLET x64-linux)
endif()

project ("btc_serverLogger_x64"
    LANGUAGES CXX
)

add_library (btc_serverLogger_x64 SHARED)

target_include_directories(btc_serverLogger_x64 PRIVATE
    "include"
)

target_sources (btc_serverLogger_x64 PRIVATE 
    "src/dllmain.cpp"
    "src/dllmain.def"
    "src/armaLogData.cpp"
    )
target_compile_features(btc_serverLogger_x64 PRIVATE cxx_std_20)

if(UNIX AND NOT APPLE)
    target_link_options(btc_serverLogger_x64 PRIVATE -static-libgcc -static-libstdc++)
endif()

find_package(nlohmann_json REQUIRED)
target_link_libraries(btc_serverLogger_x64 PRIVATE nlohmann_json::nlohmann_json)

find_package(Boost COMPONENTS algorithm range REQUIRED) 
target_link_libraries(btc_serverLogger_x64 PRIVATE Boost::algorithm Boost::range)