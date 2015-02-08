#include "serenity/net/connection.hpp"
#include "request.hpp"
#include "response.hpp"
#include "protocol_handler.hpp"

#ifndef SERENITY_HTTP_REQUEST_HANDLER_HPP_
#define SERENITY_HTTP_REQUEST_HANDLER_HPP_

namespace serenity { namespace http {

    template <class custom_handler>
    class request_handler {
        public:
            using handler_func_type = request_status (const request&, response&);
            using proto_handler = protocol_handler<custom_handler, ::serenity::net::connection<custom_handler>>;

            virtual request_status handle(const request &req, response &resp);

            void add_get(const std::string &endpoint, std::function<handler_func_type> handler) {
                get_handlers_.push_back({ endpoint, handler });
            }

        private:
            std::vector<std::pair<std::string,std::function<handler_func_type>>> get_handlers_;
    };

    template <class custom_handler>
    request_status request_handler<custom_handler>::handle(const request &req, response &resp)
    {
        std::cerr << "[handle] " << req.method << " -> " << req.uri << std::endl;
        if (req.method == "GET") {
            for (auto handler : get_handlers_) {
                std::string &h_uri = handler.first;
                if (h_uri.length() == req.uri.length() && handler.first.find(req.uri) != std::string::npos) {
                    std::cerr << "URI: " << req.uri << "  handler: " << h_uri << std::endl;
                    return handler.second(req, resp);
                }
            }
            return request_status::not_found;
        }

        return request_status::invalid_method;
    }

    
} /* serenity */ } /* http */


#endif /* end of include guard: SERENITY_HTTP_REQUEST_HANDLER_HPP_ */
