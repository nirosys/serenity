#ifndef SERENITY_HTTP_REQUEST_HPP_
#define SERENITY_HTTP_REQUEST_HPP_
#include <string>
#include <map>

#include "request_dispatcher.hpp"
#include "service_resolver.hpp"
#include "response.hpp"

namespace serenity { namespace http {

    class request {
        public:
            /** \brief Method used for request: GET, PUT, DELETE, etc. */
            std::string method;

            /** \brief The requested URI, including parameters */
            std::string uri; // Convert to uri class from cpp-net

            /** \brief All of the HTTP headers provided by the client. */
            std::map<std::string, std::string> headers;

            /** \brief Major HTTP version (ie 1) */
            uint8_t version_major;

            /** \brief Minor HTTP version (ie 0) */
            uint8_t version_minor;

            /** \brief Raw bytes of the extra data provided by the client in the request */
            std::string post_data;

            /** \brief Parses the provided data as an HTTP request, and populates the current object. */
            bool parse(const char *data, std::size_t len) {
                // TODO: Error detection, do not support malformed requests.
                int state = 0;
                std::string header;
                std::string value;
                for (const char *p = data; (p - data) < len; ++p) {
                    switch (state) {
                        case 0:
                            if (*p != ' ')
                                method = method + *p;
                            else
                                ++state;
                            break;
                        case 1:
                            if (*p != ' ')
                                uri = uri + *p;
                            else
                                ++state;
                            break;
                        case 2:
                            if (*p == '/')
                               ++state; 
                            break;
                        case 3:
                            if (*p != '.')
                                version_major = *p - '0'; // TODO: Support more than 1 digit values
                            else
                                ++state;
                            break;
                        case 4:
                            if (*p != '\r')
                                version_minor = *p - '0';
                            else
                                ++state;
                            break;
                        case 5:
                            if (*p == '\n') {
                                header = "";
                                value = "";
                                ++state;
                            }
                            else
                                return false;
                            break;
                        case 6: // Start Headers.
                            if (*p != ':' && *p != ' ')
                                header = header + *p;
                            else if (*p == ' ')
                                ++state;
                            break;
                        case 7:
                            if (*p != '\r' && *p != '\n')
                                value = value + *p;
                            else if (*p == '\n') {
                                headers.insert({ header, value });
                                ++state;
                            }
                            break;
                        case 8:
                            if (*p != '\r' && *p != '\n') {
                                header = *p;
                                value = "";
                                state = 6;
                            }
                            else if (*p == '\n') {
                                ++state;
                            }
                            break;
                        case 9:
                            if ((p - data) < len) {
                                post_data =  std::string(p, len - (p - data));
                                ++state;
                            }
                            break;
                        default:
                            break;
                    }
                }
                return (state > 0);
            }
    };
    
} /* http */ } /* serenity */

#endif /* end of include guard: SERENITY_HTTP_REQUEST_HPP_ */
