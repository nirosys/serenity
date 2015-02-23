#include "serenity/http.hpp"
#include "catch.hpp"
#include <map>

#include "common.hpp"
using namespace tests;

TEST_CASE("Verify HTTP GET Request Parsing", "[request]") {
    serenity::http::request request;
    request.add_data(requests::std_get_request, ::strlen(requests::std_get_request));

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
    request.add_data(requests::std_post_request, strlen(requests::std_post_request));
    request.parse();

    SECTION("Should parse url") {
        REQUIRE("/favicon.ico" == request.uri);
    }
    SECTION("Should parse method") {
        REQUIRE("POST" == request.method);
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

TEST_CASE("Should parse request with added parameters", "[request]") {
    serenity::http::request request;
    request.add_data(requests::std_get_parameters, strlen(requests::std_get_parameters));
    request.parse();

    SECTION("Should parse method") {
        REQUIRE("GET" == request.method);
    }
    SECTION("Should parse HTTP version") {
        REQUIRE(1 == request.version_major);
        REQUIRE(1 == request.version_minor);
    }
    SECTION("Should parse headers") {
        std::vector<std::pair<std::string,std::string>> known =
        {
            { "Host", "localhost:8084" },
            { "Connection", "keep-alive" },
            { "Accept", "*/*" },
            { "Accept-Encoding", "gzip, deflate, sdch" },
            { "Accept-Language", "en-US,en;q=0.8" }
        };

        for (auto &k : known) {
            CAPTURE(k.first);
            REQUIRE(request.headers[k.first] == k.second);
        }
    }
    SECTION("Should parse parameters") {
        REQUIRE(request.parameters.size() == 1);
        auto search = request.parameters.find("query");
        REQUIRE(request.parameters.end() != search);
        REQUIRE("asdf fdsa" == search->second);
    }
}

TEST_CASE("Should parse incorrect requests correctly", "[request]") {

    SECTION("Should parse missing end-of-lines correctly") {
        serenity::http::request request;
        request.add_data(requests::incorrect_request_00, strlen(requests::incorrect_request_00));

        REQUIRE(request.parse() == false);
        REQUIRE(request.is_error() == true);
    }

    SECTION("Should parse missing request contents") {
        serenity::http::request request;
        request.add_data(requests::incorrect_request_01, strlen(requests::incorrect_request_01));

        REQUIRE(request.parse() == false);
        REQUIRE(request.is_error() == true);
    }

}
