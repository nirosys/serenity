###############################################################################
# Serenity - Build
#  Builds Unit Tests
###############################################################################

# Sources
set(test_SRCS
      main.cc
      common.cc
      http_server.cc
      http_request.cc
      #      http_request_handler.cc
      url_policy.cc
    )

# Add includes for Catch
include_directories(
    ../external/Catch/single_include
    )

# Add test binary target
add_executable(tests ${test_SRCS})
target_link_libraries(tests curl ${Boost_LIBRARIES})

# JUnit Report Generation
add_custom_target(test_report
    COMMAND tests --reporter junit > unit_test_report.xml
    )

# Command-line Report Output... XML?... Ain't nobody got time fo dat!
add_custom_target(run_tests
    COMMAND tests --reporter console || true  # Stop build failure.. adds too much output..
    )
