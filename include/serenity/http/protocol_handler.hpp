#ifndef SERENITY_HTTP_PROTOCOL_HANDLER_HPP_
#define SERENITY_HTTP_PROTOCOL_HANDLER_HPP_

#include <iostream>
#include "serenity/net/protocol_handler.hpp"
#include "response.hpp"
#include "request.hpp"

namespace serenity { namespace http {

    const unsigned int nothing = 0;
    const unsigned int crlf_01_start = 1;
    const unsigned int crlf_01_end = 2;
    const unsigned int crlf_02_start = 3;
    const unsigned int crlf_02_end = 4;

    template <class cust_handler, class connection_type>
    class protocol_handler : public ::serenity::net::protocol_handler {
        public:
            protocol_handler() = delete;
            protocol_handler &operator=(const protocol_handler &) = delete;

            protocol_handler(connection_type &conn);

            status process(char *data, std::size_t bytes); 
            std::vector<boost::asio::const_buffer> get_response();

        private:
            response response_;
            cust_handler request_handler_;
            connection_type &connection_;
            std::array<char, 4096> data_;
            unsigned int data_end_;
            unsigned int parse_state_;
    };


    template <class cust_handler, class connection_type>
    protocol_handler<cust_handler, connection_type>::protocol_handler(connection_type &conn) :
        connection_(conn),
        parse_state_(nothing)
    {
    }

    template <class cust_handler, class connection_type>
    serenity::net::protocol_handler::status protocol_handler<cust_handler, connection_type>::process(char *data, std::size_t bytes)
    {
        if (bytes + data_end_ > data_.size())
            return serenity::net::protocol_handler::status::error;

        std::cerr << "Received:" << std::endl;
        std::cerr << std::string(data, bytes);

        memcpy(data_.data() + data_end_, data, bytes);
        data_end_ += bytes;

        char *end_of_request = nullptr;
        for (int i=0; (i < bytes) && (parse_state_ < crlf_02_end); ++i) {
            switch (parse_state_) {
                case nothing:
                case crlf_01_end:
                    if (data[i] == '\r') ++parse_state_;
                    else parse_state_ = nothing;
                    break;
                case crlf_01_start:
                case crlf_02_start:
                    if (data[i] == '\n') ++parse_state_;
                    else parse_state_ = nothing;
                    break;
                case crlf_02_end:
                    end_of_request = &data[i];
                    break;
            }
        }

        if (parse_state_ == crlf_02_end) {
            request req;
            req.parse(data_.data(), data_end_);
            data_end_ = 0;

            request_handler_.handle(req, response_);
            return status::response;
        }

        return status::no_response;
    }

    template <class cust_handler, class connection_type>
    std::vector<boost::asio::const_buffer> protocol_handler<cust_handler, connection_type>::get_response() {
        return response_.to_buffers();
    }
} };


#endif /* end of include guard: SERENITY_HTTP_PROTOCOL_HANDLER_HPP_ */
