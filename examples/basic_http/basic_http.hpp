#include <time.h>
#include <iostream>
#include "serenity/serenity.hpp"

#ifndef BASIC_HTTP_H
#define BASIC_HTTP_H

namespace examples {

    class basic_service : public serenity::http::service {
        public:
            basic_service () {
                add_get("test",
                        [](const serenity::http::request &req, serenity::http::response &resp) -> uint32_t {
                            resp.status = 200;
                            resp.content = "{ \"test\": \"success\"; }";
                            resp.headers.push_back({"Content-type", "application/json"});
                            return 0;
                        }
                );
            }
    };

}

#endif /* end of include guard: BASIC_HTTP_H */
