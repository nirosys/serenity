#ifndef SERENITY_HTTP_REQUEST_HPP_
#define SERENITY_HTTP_REQUEST_HPP_
#include <string>
#include <vector>

namespace serenity { namespace http {

    class request {
        public:
            /** Method used for request: GET, PUT, DELETE, etc. */
            std::string method;

            /** The requested URI, including parameters */
            std::string uri; // Convert to uri class from cpp-net

            /** All of the HTTP headers provided by the client. */
            std::vector<std::string> headers;

            /** Major HTTP version (ie 1) */
            uint8_t version_major;

            /** Minor HTTP version (ie 0) */
            uint8_t version_minor;

            /** Raw bytes of the extra data provided by the client in the request */
            unsigned char *post_data;

            bool parse(const char *data, std::size_t len) {
                int state = 0;
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
                        default:
                            break;
                    }
                }
                std::cerr << "[request] Method: " << method << "  uri: " << uri << std::endl;
                return (state > 0);
            }
    };
    
} /* http */ } /* serenity */

#endif /* end of include guard: SERENITY_HTTP_REQUEST_HPP_ */
