#include "monitor.hpp"

const int gPort = 8084;

namespace policies = serenity::http::policies;
using server_t = serenity::http::server<policies::url::service, policies::url::version>;

int main( int argc, char **argv ) {

    if (argc != 2) {
        std::cerr << "usage: ./" << argv[0] << " <path>" << std::endl
                  << "   Where path = path to webroot (source directory)" << std::endl
                  << std::endl;
        return -1;
    }

    server_t server(gPort);

    std::string root_dir = argv[1];

    std::cout << "Starting server on port " << gPort << std::endl;
    std::cout << "Using webroot: " << root_dir << std::endl;

    server.get_resolver().add_service<serenity::http::file_service>({"files", 1}, root_dir);
    server.get_resolver().add_service<examples::monitor_service>({"monitor", 1});

    server.run();

    server.wait_to_end();

    return 0;
}
