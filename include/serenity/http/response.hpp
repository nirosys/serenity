#include "boost/asio.hpp"
#include <vector>

#ifndef SERENITY_HTTP_RESPONSE_HPP_
#define SERENITY_HTTP_RESPONSE_HPP_

namespace serenity { namespace http {

    /** \brief Represents an HTTP header via name and value. */
    struct header {
        /** \brief The name of the header (ie. Content-type) */
        std::string name;
        /** \brief The value of the header (ie. application/json) */
        std::string value;

        header(const std::string &n, const std::string &v) : name(n), value(v) { }
    };

    /** \brief Represents an HTTP response, including headers, and content. */
    class response {
        public:
            /** \brief Converts the response into ASIO buffer(s) for transmitting to the client. */
            boost::asio::const_buffer to_buffers() {
                std::vector<boost::asio::const_buffer> buffers;
                std::stringstream stream;

                headers.push_back({ "Content-length", std::to_string(content.length()) });
                headers.push_back({ "Date", "Fri, 08 Aug 2003 08:12:31 GMT" });
                headers.push_back({ "Server", "Serenity"});

                stream << get_status_line(status);
                for (header &h : headers) {
                    stream << h.name << ": " << h.value << "\r\n";
                }
                stream << "\r\n" << content;

                response_str = stream.str();
                return boost::asio::buffer(response_str);
            }

            /** \brief Creates a string representation for the provided status code. */
            const std::string get_status_line(uint32_t status) {
                switch (status) {
                    case 200: return ok; break;
                    case 400: return bad_request; break;
                    case 404: return not_found; break;
                    case 500: return internal_error; break;
                    default:
                              return "HTTP/1.0 " + std::to_string(status) + " Unknown Response\r\n";
                              break;
                }
            }

            /** \brief The content of the response (ie. HTML data, JSON data, etc) */
            std::string content;
            /** \brief The status code of the response (ie. 200, 404) */
            int status;
            /** \brief The headers for the response */
            std::vector<header> headers;

        private:
            std::string ok = "HTTP/1.0 200 OK\r\n";
            std::string bad_request = "HTTP/1.0 400 Bad Request\r\n";
            std::string not_found = "HTTP/1.0 404 Not Found\r\n";
            std::string internal_error = "HTTP/1.0 500 Internal Server Error\r\n";
            std::string response_str = "";
    };

} /* http */ } /* serenity */


#endif /* end of include guard: SERENITY_HTTP_RESPONSE_HPP_ */
