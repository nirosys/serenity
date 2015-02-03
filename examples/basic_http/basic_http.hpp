#include <iostream>
#include "serenity/serenity.hpp"
#include <time.h>

#ifndef BASIC_HTTP_H
#define BASIC_HTTP_H

class basic_handler : public serenity::http::request_handler<basic_handler> {
    public:
        basic_handler() {
            add_get("/date",
                    [this](const serenity::http::request &req, serenity::http::response &resp)
                    {
                        struct tm *tm;
                        char str_date[100];
                        time_t t = time(NULL);
                        tm = localtime(&t);
                        strftime(str_date, sizeof(str_date), "%d %m %Y", tm);

                        resp.content = str_date;
                        resp.status = 200;
                    }
            );
            add_get("/basic",
                    [this](const serenity::http::request &req, serenity::http::response &resp)
                    {
                        std::cout << "[basic] Handler executed." << std::endl;
                        resp.status = 200;
                        resp.headers.push_back({ "Content-type", "text/html" });
                        resp.content = "<html><body><h1>BASIC</h1></body></html>";
                    }
            );

            add_get("/basic2", std::bind(&basic_handler::basic2, this,
                        std::placeholders::_1, std::placeholders::_2));
        }

        void basic2(const serenity::http::request &req, serenity::http::response &resp) {
            std::cout << "[basic2] Handler executed." << std::endl;
            resp.status = 200;
            resp.headers.push_back({ "Content-type", "application/json" });
            resp.content = "{ \"response\": \"Success!\" }";
        }
};


#endif /* end of include guard: BASIC_HTTP_H */
