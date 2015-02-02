#include "boost/asio.hpp"
#include <vector>

#ifndef SERENITY_HTTP_RESPONSE_HPP_
#define SERENITY_HTTP_RESPONSE_HPP_

namespace serenity { namespace http {

    class response {
        public:
            std::vector<boost::asio::const_buffer> to_buffers();
    };


    std::vector<boost::asio::const_buffer> response::to_buffers() {
        std::vector<boost::asio::const_buffer> buffers;
        buffers.push_back(boost::asio::buffer("HTTP/1.0 501 Not Implemented\r\n\r\n"));
        return buffers;
    }
    
} /* http */ } /* serenity */


#endif /* end of include guard: SERENITY_HTTP_RESPONSE_HPP_ */
