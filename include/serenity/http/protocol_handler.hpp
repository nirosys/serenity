#ifndef SERENITY_HTTP_PROTOCOL_HANDLER_HPP_
#define SERENITY_HTTP_PROTOCOL_HANDLER_HPP_

#include <iostream>
#include "serenity/net/protocol_handler.hpp"
#include "response.hpp"
#include "request.hpp"

namespace serenity { namespace http {

    template <class cust_handler, class connection_type>
    class protocol_handler : public ::serenity::net::protocol_handler {
        public:
            protocol_handler() = delete;
            protocol_handler &operator=(const protocol_handler &) = delete;

            protocol_handler(connection_type &conn);

            status process(char *data, std::size_t bytes); 
            std::vector<boost::asio::const_buffer> get_response();

            response response_;
        private:
            cust_handler request_handler_;
            connection_type &connection_;
    };


    template <class cust_handler, class connection_type>
    protocol_handler<cust_handler, connection_type>::protocol_handler(connection_type &conn) :
        connection_(conn)
    {
    }

    template <class cust_handler, class connection_type>
    serenity::net::protocol_handler::status protocol_handler<cust_handler, connection_type>::process(char *data, std::size_t bytes)
    {
        // buffer data... wait until it can be parsed..
        // parse data.. into request..
        // call handle on request_handler_ with parsed request.
        // write response to connection.
        std::cerr << "Received:" << std::endl
                  << data << std::endl;

        request req;
        req.method = "GET";
        req.uri = "http://localhost:8080/testblah";
        req.version_major = 1;
        req.version_minor = 0;

        request_handler_.handle(req, response_);

        return status::response;
    }

    template <class cust_handler, class connection_type>
    std::vector<boost::asio::const_buffer> protocol_handler<cust_handler, connection_type>::get_response() {
        return response_.to_buffers();
    }
} };


#endif /* end of include guard: SERENITY_HTTP_PROTOCOL_HANDLER_HPP_ */
