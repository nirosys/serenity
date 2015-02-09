#include <random>
#include <curl/curl.h>
#include <string.h>
#include <unistd.h>

#include "catch.hpp"
#include "serenity/serenity.hpp"

namespace tests {

class test_handler : public serenity::http::service {
    public:
        static std::string body_content;
        static bool handler_executed;

        test_handler() {
            //add_get("/testresult",
            //        [this](const serenity::http::request &req, serenity::http::response &res) -> serenity::http::request_status
            //        {
            //            res.status = 200;
            //            res.content = body_content;
            //            handler_executed = true;
            //            return serenity::http::request_status::ok;
            //        }
            //);
        }
};

std::string test_handler::body_content = "{ \"test_status\": \"success\"; }";
bool test_handler::handler_executed = false;


size_t ignore_body(void *ptr, size_t size, size_t nmemb, void *ignored) {
    return size * nmemb;
}

size_t gather_body(void *ptr, size_t size, size_t nmemb, char *buffer) {
    strcpy(buffer, static_cast<const char*>(ptr));
    return size * nmemb;
}

TEST_CASE("Verify server creation", "[server]") {
    std::random_device rd;
    std::uniform_int_distribution<int> dist(9000, 12000);
    int port = dist(rd);
    
    CAPTURE(port);

    serenity::http::server server(port);

    server.run();
    usleep(100);

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
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, ignore_body);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, url);

        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        REQUIRE(res == CURLE_OK);

        REQUIRE(test_handler::handler_executed == true);
    }

    SECTION("Should send data correctly.") {
        CURL *curl = curl_easy_init();
        REQUIRE(curl != nullptr);

        char buffer[256];
        char url[64];
        snprintf(url, sizeof(url), "http://localhost:%d/testresult", port);
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, gather_body);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, buffer);

        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        REQUIRE(res == CURLE_OK);

        REQUIRE(test_handler::handler_executed == true);

        REQUIRE(std::string(buffer) == test_handler::body_content);
    }

    SECTION("Should return 404 for not found endpoints") {
        CURL *curl = curl_easy_init();
        REQUIRE(curl != nullptr);

        char buffer[256];
        char url[64];
        long http_code = 0;
        snprintf(url, sizeof(url), "http://localhost:%d/testresult_NOT_FOUND", port);
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, ignore_body);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, nullptr);

        CURLcode res = curl_easy_perform(curl);
        curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);
        curl_easy_cleanup(curl);

        REQUIRE(res == CURLE_OK);
        REQUIRE(http_code == 404);
    }
}

}
