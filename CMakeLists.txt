project(serenity)
###############################################################################
# Serenity - Build
#  Builds libserenity.a
#  Builds Serenity Unit Tests
#  Builds Serenity Documentation
###############################################################################
cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
find_package(Boost 1.56.0 REQUIRED COMPONENTS system filesystem)

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
