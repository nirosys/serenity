#include <string.h>
#include "serenity/serenity.hpp"
#include "catch.hpp"
#include "common.hpp"

namespace tests {
    namespace http_request_handler {
        class ver_handler : public serenity::http::versioned_handler<ver_handler>
        {
            public:
        };
    }
}

using namespace tests;

TEST_CASE("Verify HTTP Request Handler", "[handler]") {
}

TEST_CASE("Verify HTTP Versioned Request Handler", "[handler]") {
    using namespace http_request_handler;
    serenity::http::request req;
    serenity::http::response resp;
    ver_handler handler;

    SECTION("Should not detect version on unversioned request") {
        REQUIRE(req.parse(requests::std_get_request, strlen(requests::std_get_request)));

        REQUIRE(handler.handle(req, resp) == serenity::http::request_status::not_found);

        REQUIRE(handler.version == 0);
    }

    SECTION("Should detect version on versioned request") {
        REQUIRE(req.parse(requests::std_get_versioned_request, strlen(requests::std_get_versioned_request)));

        handler.handle(req, resp);

        REQUIRE(handler.version == 1);
    }
}
