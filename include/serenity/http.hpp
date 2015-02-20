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
#include "http/service_resolver.hpp"
#include "http/service.hpp"
#include "http/policies/url.hpp"

namespace serenity {
    namespace common {
        template <class request, class response>
        class service;
    }
    namespace net {
        template <class resolver_type>
        class server;
    }
}

namespace serenity { namespace http {
    template <typename... policy_types>
    using resolver = service_resolver<request, response, policies::url::policy_set<policy_types...>>;

    template <typename... policy_types>
    using server = ::serenity::net::server<resolver<policy_types...>>;
} }

#endif /* end of include guard: SERENITY_HTTP_HPP_ */
