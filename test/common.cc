#include "common.hpp"

namespace tests {
namespace requests {
    const char *std_get_request =
        "GET /favicon.ico HTTP/1.1\r\n"
        "Host: localhost:8084\r\n"
        "Connection: keep-alive\r\n"
        "Accept: */*\r\n"
        "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_10_1) "
          "AppleWebKit/537.36 (KHTML, like Gecko) Chrome/39.0.2171.99 Safari/537.36\r\n"
        "Accept-Encoding: gzip, deflate, sdch\r\n"
        "Accept-Language: en-US,en;q=0.8\r\n"
        "\r\n";
    const char *std_get_versioned_request =
        "GET /v1/test HTTP/1.1\r\n"
        "Host: localhost:8084\r\n"
        "Connection: keep-alive\r\n"
        "Accept: */*\r\n"
        "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_10_1) "
          "AppleWebKit/537.36 (KHTML, like Gecko) Chrome/39.0.2171.99 Safari/537.36\r\n"
        "Accept-Encoding: gzip, deflate, sdch\r\n"
        "Accept-Language: en-US,en;q=0.8\r\n"
        "\r\n";
    const char *std_get_parameters =
        "GET /v1/test?query=asdf\%20fdsa HTTP/1.1\r\n"
        "Host: localhost:8084\r\n"
        "Connection: keep-alive\r\n"
        "Accept: */*\r\n"
        "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_10_1) "
          "AppleWebKit/537.36 (KHTML, like Gecko) Chrome/39.0.2171.99 Safari/537.36\r\n"
        "Accept-Encoding: gzip, deflate, sdch\r\n"
        "Accept-Language: en-US,en;q=0.8\r\n"
        "\r\n";

    const char *std_get_path_parameters =
        "GET /v1/test/extra/params HTTP/1.1\r\n"
        "Host: localhost:8084\r\n"
        "Connection: keep-alive\r\n"
        "Accept: */*\r\n"
        "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_10_1) "
          "AppleWebKit/537.36 (KHTML, like Gecko) Chrome/39.0.2171.99 Safari/537.36\r\n"
        "Accept-Encoding: gzip, deflate, sdch\r\n"
        "Accept-Language: en-US,en;q=0.8\r\n"
        "\r\n";
    
    const char *std_post_request =
        "POST /favicon.ico HTTP/1.1\r\n"
        "Host: localhost:8084\r\n"
        "Connection: keep-alive\r\n"
        "Accept: */*\r\n"
        "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_10_1) "
          "AppleWebKit/537.36 (KHTML, like Gecko) Chrome/39.0.2171.99 Safari/537.36\r\n"
        "Accept-Encoding: gzip, deflate, sdch\r\n"
        "Accept-Language: en-US,en;q=0.8\r\n"
        "Content-Length: 27\r\n"
        "\r\n"
        "{\"post_data\": \"DataData\"; }";

    const char *incorrect_request_00 = // Missing end-of-lines
        "GET /URL"
        "Host: localhost:8084"
        "Connection: keep-alive"
        "Accept: */*\r\n"
        "\r\n";

    const char *incorrect_request_01 = // Missing HTTP version
        "GET /URL\r\n"
        "Host: localhost:8084\r\n"
        "Connection: keep-alive\r\n"
        "Accept: */*\r\n"
        "\r\n";
} }
