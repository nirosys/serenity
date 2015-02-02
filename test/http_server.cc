#include <random>
#include <curl/curl.h>

#include "catch.hpp"
#include "serenity/serenity.hpp"

class test_handler : public serenity::http::request_handler<test_handler> {
    public:
        static bool handler_executed;

        test_handler() {
            //add_get("testresult",
            //        [this](const serenity::http::request &req, serenity::http::response &res) -> void
            //        {
            //        }
            //);
        }

        void handle(const serenity::http::request &req, serenity::http::response &resp) override
        {
            handler_executed = true;
        }
};

bool test_handler::handler_executed = false;



TEST_CASE("Verify server creation", "[server]") {
    std::random_device rd;
    std::uniform_int_distribution<int> dist(9000, 12000);
    int port = dist(rd);
    bool handler_executed = false;
    
    CAPTURE(port);

    serenity::net::server<test_handler> server(port);

    //server.add_get("/testresult",
    //    [&handler_executed](const serenity::http::request &req, serenity::http::response &res) -> int
    //    {
    //        handler_executed = true;
    //        return 200;
    //    }
    //);

    server.run();

    SECTION("Should be listening") {
        REQUIRE(server.is_running() == true);
    }
    SECTION("Should be responding") {
        CURL *curl = curl_easy_init();
        REQUIRE(curl != nullptr);

        char url[32];
        snprintf(url, 32, "http://localhost:%d/", port);
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_NOBODY, 1);

        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        REQUIRE(res == CURLE_OK); // Do not care about error codes here..
                                  // Server will probably return 404 for
                                  // endpoint not found.
    }
    SECTION("Should execute handlers") {
        CURL *curl = curl_easy_init();
        REQUIRE(curl != nullptr);

        char url[64];
        snprintf(url, sizeof(url), "http://localhost:%d/testresult", port);
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_NOBODY, 1);

        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        REQUIRE(test_handler::handler_executed == true);
    }
}

