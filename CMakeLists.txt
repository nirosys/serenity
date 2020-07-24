project(serenity)
###############################################################################
# Serenity - Build
#  Builds Serenity Examples
#  Builds Serenity Unit Tests
#  Builds Serenity Documentation
###############################################################################
cmake_minimum_required(VERSION 2.8)

set(Boost_USE_MULTITHREADED ON)
find_package(Boost 1.69.0 REQUIRED COMPONENTS system filesystem thread)
find_package(Threads REQUIRED)

if (CMAKE_THREAD_LIBS_INIT)
    set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} ${CMAKE_THREAD_LIBS_INIT}")
endif(CMAKE_THREAD_LIBS_INIT)

include(CheckCXXCompilerFlag)
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    check_cxx_compiler_flag("-std=c++11" HAS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS}")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    # Dunno what to do here yet.
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    check_cxx_compiler_flag("-std=c++11" HAS_CXX11)
    check_cxx_compiler_flag("-stdlib=libc++" HAS_LIBCXX)
    check_cxx_compiler_flag("-stdlib=libstdc++" HAS_LIBSTDCXX)
    if (HAS_LIBCXX)
        message("Using libc++")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++")
        set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -lc++")
    elseif(HAS_LIBSTDCXX)
        message("Using libstdc++")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libstdc++")
        set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -lstdc++")
    endif()
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
    check_cxx_compiler_flag("-std=c++11" HAS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++")
endif()

if (NOT HAS_CXX11)
    message(FATAL_ERROR "Serenity requires a C++ compiler that supports C++11.")
endif()


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
