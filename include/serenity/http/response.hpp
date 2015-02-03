#include "boost/asio.hpp"
#include <vector>

#ifndef SERENITY_HTTP_RESPONSE_HPP_
#define SERENITY_HTTP_RESPONSE_HPP_

namespace serenity { namespace http {

    struct header {
        std::string name;
        std::string value;
        header(const std::string &n, const std::string &v) : name(n), value(v) { }
    };

    class response {
        public:
            std::vector<boost::asio::const_buffer> to_buffers();

            std::string content;
            int status;
            std::vector<header> headers;
            std::string response_str;
    };

    std::vector<boost::asio::const_buffer> response::to_buffers() {
        std::vector<boost::asio::const_buffer> buffers;
        std::stringstream stream;

        headers.push_back({ "Content-length", std::to_string(content.length()) });
        headers.push_back({ "Date", "Fri, 08 Aug 2003 08:12:31 GMT" });
        headers.push_back({ "Server", "Serenity"});

        //buffers.push_back(boost::asio::buffer("HTTP/1.0 200 OK\r\n"));
        stream << "HTTP/1.0 " << status << "\r\n";
        for (header &h : headers) {
            stream << h.name << ": " << h.value << "\r\n";
        }
        stream << "\r\n" << content;
        
        response_str = stream.str();

        buffers.push_back(boost::asio::buffer(response_str));
        return buffers;
    }
    
} /* http */ } /* serenity */


#endif /* end of include guard: SERENITY_HTTP_RESPONSE_HPP_ */
