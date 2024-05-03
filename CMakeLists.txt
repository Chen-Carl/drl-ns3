cmake_minimum_required(VERSION 3.10)

include(FetchContent)

# ============== C++ Compiler ==============
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# SET FLAGS
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

if(CMAKE_CXX_COMPILER)
    message("system C++ compiler: ${CMAKE_CXX_COMPILER}")
else()
    find_program(CXX_COMPILER_PATH NAMES g++)
    if(CXX_COMPILER_PATH)
        message("found C++ compiler: ${CXX_COMPILER_PATH}")
        set(CMAKE_CXX_COMPILER ${CXX_COMPILER_PATH})
    else()
        message(FATAL_ERROR "C++ compiler not found. Set -DCMAKE_CXX_COMPILER=/path/to/g++ to specify the compiler.")
    endif()
endif()

# ================= project =================
project(drl-ns3-simulation LANGUAGES CXX)

include_directories(src)

# ========== third party libraries ==========

# spdlog
message("[fetch third party] spdlog")
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.14.0
)
FetchContent_Populate(spdlog)
add_subdirectory(${spdlog_SOURCE_DIR} ${spdlog_BINARY_DIR})

include_directories(build/_deps/spdlog-src/include)

# =============== system ns-3 ===============
include_directories(/home/cll/software/ns-allinone-3.41/include)

set(ENV{PKG_CONFIG_PATH} "/home/cll/software/ns-allinone-3.41/lib/pkgconfig")
find_package(PkgConfig)

pkg_search_module(ns3-core REQUIRED ns3-core)
pkg_search_module(ns3-point-to-point REQUIRED ns3-point-to-point)
pkg_search_module(ns3-internet REQUIRED ns3-internet)
pkg_search_module(ns3-applications REQUIRED ns3-applications)
pkg_search_module(ns3-network REQUIRED ns3-network)
pkg_search_module(ns3-csma REQUIRED ns3-csma)

set(NS3_LIBS 
    ${ns3-core_LDFLAGS} 
    ${ns3-point-to-point_LDFLAGS} 
    ${ns3-internet_LDFLAGS} 
    ${ns3-applications_LDFLAGS} 
    ${ns3-network_LDFLAGS} 
    ${ns3-csma_LDFLAGS}
)

# =============== executable ===============

add_executable(testSpdlog tests/testSpdlog.cpp)
target_link_libraries(testSpdlog PRIVATE spdlog::spdlog)

add_executable(testNs3 tests/testNs3.cpp)
target_link_libraries(testNs3 PRIVATE ${NS3_LIBS})

add_executable(testRateLimiter 
    tests/testRateLimiter.cpp 
    src/config.cpp 
    src/rpc-server.cpp
    src/rpc-server-helper.cpp
    src/rpc-client.cpp
    src/rpc-client-helper.cpp
)
target_link_libraries(testRateLimiter PRIVATE spdlog::spdlog ${NS3_LIBS})

add_executable(main 
    tests/main.cpp 
    src/config.cpp 
    src/rpc-server.cpp
    src/rpc-server-helper.cpp
    src/rpc-client.cpp
    src/rpc-client-helper.cpp
)
target_link_libraries(main PRIVATE ${NS3_LIBS})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
