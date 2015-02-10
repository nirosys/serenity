#include <iostream>
#include "serenity/serenity.hpp"
#include <time.h>

#ifndef BASIC_HTTP_H
#define BASIC_HTTP_H

namespace examples {

class basic_handler : public serenity::http::service {
    public:
        basic_handler() {
            //add_get("/basic",
            //        [this](const serenity::http::request &req, serenity::http::response &resp) -> serenity::http::request_status
            //        {
            //            std::cout << "[basic] Handler executed." << std::endl;
            //            resp.status = 200;
            //            resp.headers.push_back({ "Content-type", "text/html" });
            //            resp.content = "<html><body><h1>BASIC</h1></body></html>";
            //            return serenity::http::request_status::ok;
            //        }
            //);

            //add_get("/basic2", std::bind(&basic_handler::basic2, this,
            //            std::placeholders::_1, std::placeholders::_2));
        }

        //serenity::http::request_status basic2(const serenity::http::request &req, serenity::http::response &resp) {
        //    std::cout << "[basic2] Handler executed." << std::endl;
        //    resp.status = 200;
        //    resp.headers.push_back({ "Content-type", "application/json" });
        //    resp.content = "{ \"response\": \"Success!\" }";
        //    return serenity::http::request_status::ok;
        //}
};

}

#endif /* end of include guard: BASIC_HTTP_H */
