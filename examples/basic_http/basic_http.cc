#include "serenity/serenity.hpp"
const int gPort = 8084;

namespace policies = serenity::http::policies;
using server_t = serenity::http::server<policies::url::service, policies::url::version>;
using service_t = serenity::http::service;

int main( int argc, char **argv ) {
    server_t server(gPort);

    std::cout << "Starting server on port " << gPort << std::endl;

    service_t &service = server.get_resolver()
        .add_service<service_t>(std::make_tuple<std::string,int>("basic",1));
    service.add_get("test",
            [](const serenity::http::request &req, serenity::http::response &resp) -> uint32_t {
                resp.status = 200;
                resp.content = "{ \"test\": \"success\" }";
                resp.headers.push_back({"Content-type", "application/json"});
                return 0;
            }
    );


    server.run();

    server.wait_to_end();

    return 0;
}
