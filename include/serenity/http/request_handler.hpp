#include "serenity/net/connection.hpp"
#include "request.hpp"
#include "response.hpp"

#ifndef SERENITY_HTTP_REQUEST_HANDLER_HPP_
#define SERENITY_HTTP_REQUEST_HANDLER_HPP_

namespace serenity { namespace http {

    template <class custom_handler>
    class request_handler {
        public:
            using proto_handler = protocol_handler<custom_handler, ::serenity::net::connection<custom_handler>>;

            void handle(const request &req, response &resp);

            void add_get(const std::string &endpoint, std::function<void(const request&,response&)> handler) {
                get_handlers_.push_back({ endpoint, handler });
            }

        private:
            std::vector<std::pair<std::string,std::function<void (const request &, response &)>>> get_handlers_;
    };

    template <class custom_handler>
    void request_handler<custom_handler>::handle(const request &req, response &resp)
    {
        if (req.method == "GET") {
            for (auto handler : get_handlers_) {
                std::string &h_uri = handler.first;
                if (h_uri.length() == req.uri.length() && handler.first.find(req.uri) != std::string::npos) {
                    std::cerr << "URI: " << req.uri << "  handler: " << h_uri << std::endl;
                    handler.second(req, resp);
                    return;
                }
            }
        }
    }

    
} /* serenity */ } /* http */


#endif /* end of include guard: SERENITY_HTTP_REQUEST_HANDLER_HPP_ */
