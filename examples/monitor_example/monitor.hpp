#include <time.h>
#include <iostream>
#include <random>
#include "serenity/serenity.hpp"

#ifndef MONITOR_EXAMPLE_H
#define MONITOR_EXAMPLE_H

namespace examples {
    class monitor_service : public serenity::http::service {
        public:
            monitor_service() : gen_(rd_()), dist_(0, 100) {
                add_get("random",
                        [this](const serenity::http::request &req, serenity::http::response &resp) -> uint32_t
                        {
                            resp.status = 200;
                            resp.content =
                               "{ \"data\": " + std::to_string(dist_(gen_)) + " }";
                            return 0;
                        }
                );

                add_get("load",
                        [this](const serenity::http::request &req, serenity::http::response &resp) -> uint32_t
                        {
                            double load_1min = 0.0;
                            if (getloadavg(&load_1min, 1) == 1) {
                                resp.status = 200;
                                resp.content = "{ \"load_1min\": " + std::to_string(load_1min) + " }";
                            }
                            else {
                                resp.status = 500;
                                resp.content = "";
                            }
                            return 0;
                        }
                );
            }
        private:
            std::random_device rd_;
            std::mt19937 gen_;
            std::uniform_int_distribution<> dist_;

    };
}

#endif /* end of include guard: MONITOR_EXAMPLE_H */
