

set(basic_http_SRCS
    basic_http.cc
    )

add_executable(basic_http ${basic_http_SRCS})
target_link_libraries(basic_http ${Boost_LIBRARIES})
