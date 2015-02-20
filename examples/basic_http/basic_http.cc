#include "basic_http.hpp"

const int gPort = 8084;

namespace policies = serenity::http::policies;
using server_t = serenity::http::server<policies::url::service, policies::url::version>;

using namespace examples;
int main( int argc, char **argv ) {
    server_t server(gPort);

    std::cout << "Starting server on port " << gPort << std::endl;

    server.get_resolver().add_service<basic_service>({"basic", 1});

    server.run();

    server.wait_to_end();

    return 0;
}
