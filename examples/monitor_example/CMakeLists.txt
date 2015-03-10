

set(monitor_SRCS
    monitor.cc
    )

add_executable(monitor ${monitor_SRCS})
target_link_libraries(monitor ${Boost_LIBRARIES})
