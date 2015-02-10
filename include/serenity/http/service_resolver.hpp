#ifndef SERENITY_HTTP_SERVICE_RESOLVER_HPP_
#define SERENITY_HTTP_SERVICE_RESOLVER_HPP_

#include "serenity/common/service.hpp"
#include "serenity/common/service_resolver.hpp"

namespace serenity { namespace http {

    template <class request_type, class response_type>
    class service_resolver : public common::service_resolver<request_type, response_type> {
        public:
            service_resolver(const service_resolver &) = delete;
            service_resolver &operator=(const service_resolver &) = delete;

            service_resolver() {}

            virtual typename common::service_resolver<request_type, response_type>::handler resolve(const request_type &) override;
    };


    template <class request_type, class response_type>
    typename common::service_resolver<request_type,response_type>::handler service_resolver<request_type, response_type>::resolve(const request_type &req)
    {
        return [](const request_type &, response_type &) -> uint32_t { return 0; };
    }


    
} /* http */ } /* serenity */


#endif /* end of include guard: SERENITY_HTTP_SERVICE_RESOLVER_HPP_ */
