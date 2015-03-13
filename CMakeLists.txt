project(serenity)
###############################################################################
# Serenity - Build
#  Builds Serenity Examples
#  Builds Serenity Unit Tests
#  Builds Serenity Documentation
###############################################################################
cmake_minimum_required(VERSION 2.8)

include(CheckCXXCompilerFlag)
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    check_cxx_compiler_flag("-std=c++11" HAS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    # Dunno what to do here yet.
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    check_cxx_compiler_flag("-std=c++11" HAS_CXX11)
    check_cxx_compiler_flag("-stdlib=libc++" HAS_LIBCXX)
    check_cxx_compiler_flag("-stdlib=libstdc++" HAS_LIBSTDCXX)
    if (HAS_LIBCXX)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++")
        set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -lc++ -lpthread")
    elseif(HAS_LIBSTDCXX)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libstdc++")
    endif()
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
    check_cxx_compiler_flag("-std=c++11" HAS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++")
endif()

if (NOT HAS_CXX11)
    message(FATAL_ERROR "Serenity requires a C++ compiler that supports C++11.")
endif()

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
find_package(Boost 1.55.0 REQUIRED COMPONENTS system filesystem)

include_directories(include ${Boost_INCLUDE_DIRS})

# Unit Tests
add_subdirectory(test)
# Documentation
add_subdirectory(docs)
# Examples
add_subdirectory(examples)
# Spikes
add_subdirectory(spikes)

# Enable 'make test'
#enable_testing()
#add_test(NAME SerenityTest COMMAND tests --reporter compact)
