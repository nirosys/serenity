#include "basic_http.hpp"

const int gPort = 8084;

int main( int argc, char **argv ) {
    serenity::net::server<basic_handler> server(gPort);

    std::cout << "Starting server on port " << gPort << std::endl;

    server.run();

    server.wait_to_end();


    return 0;
}
