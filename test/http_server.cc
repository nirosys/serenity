#include <random>
#include <curl/curl.h>
#include <string.h>
#include <unistd.h>

#include "catch.hpp"
#include "serenity/serenity.hpp"

namespace tests {

    namespace v1 {
        class test_handler : public serenity::http::service {
            public:
                static std::string body_content;
                static bool handler_executed;

                test_handler() {
                    add_get("testresult",
                            [this](const serenity::http::request &req, serenity::http::response &res) -> uint32_t
                            {
                                res.status = 200;
                                res.content = body_content;
                                handler_executed = true;
                                return 0;
                            }
                           );
                }
        };
        std::string test_handler::body_content = "{ \"test_status\": \"success\"; \"version\": 1; }";
        bool test_handler::handler_executed = false;
    }
    namespace v2 {
        class test_handler : public serenity::http::service {
            public:
                static std::string body_content;
                static bool handler_executed;

                test_handler() {
                    add_get("testresult",
                            [this](const serenity::http::request &req, serenity::http::response &res) -> uint32_t
                            {
                                res.status = 200;
                                res.content = body_content;
                                handler_executed = true;
                                return 0;
                            }
                           );
                }
        };
        std::string test_handler::body_content = "{ \"test_status\": \"success\"; \"version\": 2; }";
        bool test_handler::handler_executed = false;
    }



size_t ignore_body(void *ptr, size_t size, size_t nmemb, void *ignored) {
    return size * nmemb;
}

size_t gather_body(void *ptr, size_t size, size_t nmemb, char *buffer) {
    strcpy(buffer, static_cast<const char*>(ptr));
    return size * nmemb;
}

CURLcode fetch_url(const std::string &url, long &code, std::string &body) {
    CURL *curl = curl_easy_init();
    REQUIRE(curl != nullptr);

    char buffer[256];
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, gather_body);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, buffer);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &code);
    curl_easy_cleanup(curl);

    body = buffer;

    return res;
}

TEST_CASE("Verify server creation", "[server]") {
    std::random_device rd;
    std::uniform_int_distribution<int> dist(9000, 12000);
    int port = dist(rd);
    
    CAPTURE(port);

    serenity::http::server<> server(port);

    server.get_resolver().add_service<v1::test_handler>({});

    server.run();
    usleep(1000);

    SECTION("Should be listening") {
        REQUIRE(server.is_running() == true);
    }
    SECTION("Should be responding") {
        long http_code = 0;
        std::string body;

        CURLcode res = fetch_url("http://127.0.0.1:" + std::to_string(port) + "/", http_code, body);

        REQUIRE(res == CURLE_OK); // Do not care about error codes here..
                                  // Server will probably return 404 for
                                  // endpoint not found.
    }
    SECTION("Should execute handlers") {
        long http_code = 0;
        std::string body;

        CURLcode res = fetch_url("http://127.0.0.1:" + std::to_string(port) + "/testresult", http_code, body);

        REQUIRE(res == CURLE_OK);

        REQUIRE(v1::test_handler::handler_executed == true);
    }

    SECTION("Should send data correctly.") {
        long http_code = 0;
        std::string body;

        CURLcode res = fetch_url("http://127.0.0.1:" + std::to_string(port) + "/testresult", http_code, body);

        REQUIRE(res == CURLE_OK);

        REQUIRE(v1::test_handler::handler_executed == true);

        REQUIRE(body == v1::test_handler::body_content);
    }

    SECTION("Should return 404 for not found endpoints") {
        long http_code = 0;
        std::string body;

        CURLcode res = fetch_url("http://127.0.0.1:" + std::to_string(port) + "/NOT_FOUND", http_code, body);

        REQUIRE(res == CURLE_OK);
        REQUIRE(http_code == 404);
    }
}

TEST_CASE("Verify versioned service", "[server]") {
    std::random_device rd;
    std::uniform_int_distribution<int> dist(9000, 12000);
    int port = dist(rd);

    CAPTURE(port);

    serenity::http::server<serenity::http::policies::url::version> server(port);

    server.get_resolver().add_service<v1::test_handler>({1});
    server.get_resolver().add_service<v2::test_handler>({2});

    server.run();
    usleep(1000);

    SECTION("Verify v1 Call") {
        long http_code = 0;
        std::string body;

        CURLcode res = fetch_url("http://127.0.0.1:" + std::to_string(port) + "/v1/testresult", http_code, body);

        REQUIRE(res == CURLE_OK);
        REQUIRE(http_code == 200);
        REQUIRE(body == v1::test_handler::body_content);
    }
    SECTION("Verify v2 Call") {
        long http_code = 0;
        std::string body;

        CURLcode res = fetch_url("http://127.0.0.1:" + std::to_string(port) + "/v2/testresult", http_code, body);

        REQUIRE(res == CURLE_OK);
        REQUIRE(http_code == 200);
        REQUIRE(body == v2::test_handler::body_content);
    }
    SECTION("Verify v1 & v2 Call") {
        std::string body;
        long http_code = 0;

        CURLcode res = fetch_url("http://127.0.0.1:" + std::to_string(port) + "/v1/testresult", http_code, body);

        REQUIRE(res == CURLE_OK);
        REQUIRE(http_code == 200);
        REQUIRE(body == v1::test_handler::body_content);

        res = fetch_url("http://127.0.0.1:" + std::to_string(port) + "/v2/testresult", http_code, body);

        REQUIRE(res == CURLE_OK);
        REQUIRE(http_code == 200);
        REQUIRE(body == v2::test_handler::body_content);
    }
}

}
