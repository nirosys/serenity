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

            virtual void handle(const request &req, response &resp) = 0;
    };

    
} /* serenity */ } /* http */


#endif /* end of include guard: SERENITY_HTTP_REQUEST_HANDLER_HPP_ */
