#ifndef SERENITY_HTTP_HPP_
#define SERENITY_HTTP_HPP_


namespace serenity { namespace http {

    enum class request_status {
        ok,                      // All is well, return response.
        internal_error,          // Exception thrown.. or other unknown error.
        parse_error,             // Misc. parse error.. invalid request.
        invalid_uri,             // Parse error specifically with URI.
        not_found,               // Requested endpoint is not found.
        invalid_method,          // Provided method is unknown.
    };
} /* http */ } /* serenity */

#include "http/request.hpp"
#include "http/response.hpp"
#include "http/request_handler.hpp"
#include "http/versioned_handler.hpp"
#include "http/protocol_handler.hpp"
#include "net/server.hpp"

#endif /* end of include guard: SERENITY_HTTP_HPP_ */
