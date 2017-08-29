###############################################################################
# Serenity - Build
#  Builds Unit Tests
###############################################################################

if ("${SERENITY_TEST_IPV6}" STREQUAL "")
   set(SERENITY_TEST_IPV6 TRUE)
endif("${SERENITY_TEST_IPV6}" STREQUAL "")

# Sources
set(test_SRCS
      main.cc
      common.cc
      http_server.cc
      http_request.cc
      url_policy.cc
    )

# Add includes for Catch
include_directories(
    ../external/Catch/single_include
    )

if (NOT EXISTS "../external/Catch/single_include/catch.hpp")
    message("Catch not found, not building unit tests. Run `git submodule update --init` to build tests.") 
else()
    # Add test binary target
    add_executable(tests ${test_SRCS})
    target_link_libraries(tests curl ${Boost_LIBRARIES})

    if(SERENITY_TEST_IPV6)
       set(SERENITY_TEST_FILTER "")
    else(SERENITY_TEST_IPV6)
       set(SERENITY_TEST_FILTER "~[ipv6]")
    endif(SERENITY_TEST_IPV6)

    # JUnit Report Generation
    add_custom_target(test_report
       COMMAND tests --reporter junit "${SERENITY_TEST_FILTER}" > unit_test_report.xml
       )

    # Command-line Report Output... XML?... Ain't nobody got time fo dat!
    add_custom_target(run_tests
       COMMAND tests --reporter console "${SERENITY_TEST_FILTER}" || true  # Stop build failure.. adds too much output..
       )
endif()

