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
                    add_get("cause_exception",
                            [this](const serenity::http::request &req, serenity::http::response &res) -> uint32_t
                            {
                                throw std::runtime_error("This is intended..");
                            }
                           );
                    add_get("extra",
                            [this](const serenity::http::request &req, serenity::http::response &res) -> uint32_t
                            {
                                res.status = 200;
                                res.content = "{\"function\": \"" + req.function + "\", \"extra\": \"" + req.extra_path + "\" }";
                                res.headers.push_back({"Content-type", "application/json"});
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

    serenity::http::server<> server;

    server.set_port(port);

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

    serenity::http::server<serenity::http::policies::url::version> server;

    server.set_port(port);

    server.get_resolver().add_service<v1::test_handler>(std::make_tuple<int>(1));
    server.get_resolver().add_service<v2::test_handler>(std::make_tuple<int>(2));

    server.run();
    usleep(1000);

    SECTION("Should return 200, with data, on correct v1 call") {
        long http_code = 0;
        std::string body;

        CURLcode res = fetch_url("http://127.0.0.1:" + std::to_string(port) + "/v1/testresult", http_code, body);

        REQUIRE(res == CURLE_OK);
        REQUIRE(http_code == 200);
        REQUIRE(body == v1::test_handler::body_content);
    }
    SECTION("Should return 200, with data, on correct v2 call") {
        long http_code = 0;
        std::string body;

        CURLcode res = fetch_url("http://127.0.0.1:" + std::to_string(port) + "/v2/testresult", http_code, body);

        REQUIRE(res == CURLE_OK);
        REQUIRE(http_code == 200);
        REQUIRE(body == v2::test_handler::body_content);
    }
    SECTION("Should return 200, with data, on correct, v1 & v2 calls in sequence") {
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
    SECTION("Should return 404 on improper URI request") {
        std::string body;
        long http_code = 0;

        CURLcode res = fetch_url("http://127.0.0.1:" + std::to_string(port) + "/", http_code, body);

        REQUIRE(res == CURLE_OK);
        REQUIRE(http_code == 404);
    }
    SECTION("Should return 404 on unmatched endpoint within service") {
        std::string body;
        long http_code = 0;

        CURLcode res = fetch_url("http://127.0.0.1:" + std::to_string(port) + "/v2/NOT_FOUND", http_code, body);

        REQUIRE(res == CURLE_OK);
        REQUIRE(http_code == 404);
    }
}

TEST_CASE("Verify error handling", "[server]") {
    std::random_device rd;
    std::uniform_int_distribution<int> dist(9000, 12000);
    int port = dist(rd);

    CAPTURE(port);

    serenity::http::server<serenity::http::policies::url::version> server(port);

    server.get_resolver().add_service<v1::test_handler>(std::make_tuple<int>(1));

    server.run();
    usleep(1000);

    SECTION("Should return 500 status when exception thrown in handler") {
        std::string body;
        long http_code = 0;

        CURLcode res = fetch_url("http://127.0.0.1:" + std::to_string(port) + "/v1/cause_exception", http_code, body);
        REQUIRE(res == CURLE_OK);
        REQUIRE(http_code == 500);
    }
}

TEST_CASE("Verify extra path parameters", "[server]") {
    std::random_device rd;
    std::uniform_int_distribution<int> dist(9000, 12000);
    int port = dist(rd);

    CAPTURE(port);

    serenity::http::server<serenity::http::policies::url::version> server(port);

    server.get_resolver().add_service<v1::test_handler>(std::make_tuple(1));

    server.run();
    usleep(1000);

    SECTION("Should parse function name") {
        std::string body;
        long http_code = 0;

        CURLcode res = fetch_url("http://127.0.0.1:" + std::to_string(port) + "/v1/extra", http_code, body);
        REQUIRE(res == CURLE_OK);
        REQUIRE(http_code == 200);
        REQUIRE(body == "{\"function\": \"extra\", \"extra\": \"\" }");
    }

    SECTION("Should parse extra path parameters") {
        std::string body;
        long http_code = 0;

        CURLcode res = fetch_url("http://127.0.0.1:" + std::to_string(port) + "/v1/extra/path/here?foo=/bar", http_code, body);
        REQUIRE(res == CURLE_OK);
        REQUIRE(http_code == 200);
        REQUIRE(body == "{\"function\": \"extra\", \"extra\": \"/path/here\" }");
    }
}

TEST_CASE("Verify listening on specific IP addresses (IPv6)", "[server]") {
    std::random_device rd;
    std::uniform_int_distribution<int> dist(9000, 12000);
    int port = dist(rd);

    CAPTURE(port);

    serenity::http::server<serenity::http::policies::url::version> server("::1", port);

    server.get_resolver().add_service<v1::test_handler>(std::make_tuple<int>(1));

    server.run();
    usleep(1000);

    SECTION("Should not connect to localhost (IPv4)") {
        std::string body;
        long http_code = 0;

        CURLcode res = fetch_url("http://127.0.0.1:" + std::to_string(port) + "/v1/testresult", http_code, body);
        REQUIRE(res == CURLE_COULDNT_CONNECT);
    }

    SECTION("Should connect to localhost (IPv6)") {
        std::string body;
        long http_code = 0;

        CURLcode res = fetch_url("http://[::1]:" + std::to_string(port) + "/v1/testresult", http_code, body);
        REQUIRE(res == CURLE_OK);
        REQUIRE(http_code == 200);
    }

}

TEST_CASE("Verify listening on specific IP addresses (IPv4)", "[server]") {
    std::random_device rd;
    std::uniform_int_distribution<int> dist(9000, 12000);
    int port = dist(rd);

    CAPTURE(port);

    serenity::http::server<serenity::http::policies::url::version> server(port);
    server.set_address("127.0.0.1");

    server.get_resolver().add_service<v1::test_handler>(std::make_tuple<int>(1));

    server.run();
    usleep(1000);

    SECTION("Should connect to localhost (IPv4)") {
        std::string body;
        long http_code = 0;

        CURLcode res = fetch_url("http://127.0.0.1:" + std::to_string(port) + "/v1/testresult", http_code, body);
        REQUIRE(res == CURLE_OK);
        REQUIRE(http_code == 200);
    }

    SECTION("Should not connect to localhost (IPv6)") {
        std::string body;
        long http_code = 0;

        CURLcode res = fetch_url("http://[::1]:" + std::to_string(port) + "/v1/testresult", http_code, body);
        REQUIRE(res == CURLE_COULDNT_CONNECT);
    }
}
}
