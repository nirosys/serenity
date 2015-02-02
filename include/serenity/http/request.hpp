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
    };
    
} /* http */ } /* serenity */

#endif /* end of include guard: SERENITY_HTTP_REQUEST_HPP_ */
