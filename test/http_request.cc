#include "serenity/http.hpp"
#include "catch.hpp"
#include <map>

#include "common.hpp"
using namespace tests;

TEST_CASE("Verify HTTP GET Request Parsing", "[request]") {
    serenity::http::request request;
    request.parse(requests::std_get_request, strlen(requests::std_get_request));
    SECTION("Can parse method") {
        REQUIRE("GET" == request.method);
    }
    SECTION("Can parse URI") {
        REQUIRE("/favicon.ico" == request.uri);
    }
    SECTION("Can Parse HTTP Version") {
        REQUIRE(1 == request.version_major);
        REQUIRE(1 == request.version_minor);
    }
    SECTION("Can parse headers") {
        std::vector<std::pair<std::string,std::string>> known =
        {
            { "Host", "localhost:8084" },
            { "Connection", "keep-alive" },
            { "Accept", "*/*" },
            { "Accept-Encoding", "gzip, deflate, sdch" },
            { "Accept-Language", "en-US,en;q=0.8" }
        };

        for (auto &k : known) {
            REQUIRE(request.headers[k.first] == k.second);
        }
    }
}

TEST_CASE("Verify HTTP POST Request Parsing", "[request]") {
    serenity::http::request request;
    request.parse(requests::std_post_request, strlen(requests::std_post_request));

    SECTION("Can Parse Method") {
        REQUIRE("/favicon.ico" == request.uri);
    }
    SECTION("Can Parse HTTP Version") {
        REQUIRE(1 == request.version_major);
        REQUIRE(1 == request.version_minor);
    }
    SECTION("Can parse headers") {
        std::vector<std::pair<std::string,std::string>> known =
        {
            { "Host", "localhost:8084" },
            { "Connection", "keep-alive" },
            { "Accept", "*/*" },
            { "Accept-Encoding", "gzip, deflate, sdch" },
            { "Accept-Language", "en-US,en;q=0.8" }
        };

        for (auto &k : known) {
            REQUIRE(request.headers[k.first] == k.second);
        }
    }
    SECTION("Can parse post data") {
        REQUIRE(request.post_data == "{\"post_data\": \"DataData\"; }");
    }
}
