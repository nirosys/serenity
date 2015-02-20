#ifndef SERENITY_HTTP_SERVICE_RESOLVER_HPP_
#define SERENITY_HTTP_SERVICE_RESOLVER_HPP_

#include <map>
#include "serenity/http/service.hpp"
#include "serenity/common/service_resolver.hpp"
#include "policies/url.hpp"

namespace serenity { namespace http {

    template <class request_type, class response_type, typename policy_type>
    class service_resolver {
        public:
            using request = request_type;
            using response = response_type;
            using service = service;
            using handler = typename common::service_resolver<request_type, response_type>::handler;
            service_resolver(const service_resolver &) = delete;
            service_resolver &operator=(const service_resolver &) = delete;

            service_resolver() {}

            bool resolve(request_type &, service &);

            template <class service_type>
            void add_service(typename policy_type::key_type key) {
                services_[key] = std::unique_ptr<service>(new service_type());
//                std::cerr << "[resolver] Adding" << std::endl;
            }

        private:
            policy_type policies_;
            std::map<typename policy_type::key_type, std::unique_ptr<service>> services_;
    };

    template <class request_type, class response_type, typename policy_type>
    bool service_resolver<request_type, response_type, policy_type>::resolve(request_type &req, service &svc)
    {
        std::string remaining;
        if (policies_.apply(req.uri, remaining)) {
            auto search = services_.find(policies_.values);
            if (search != services_.end()) {
                svc = *(search->second);
                req.uri = remaining;
                return true;
            }
            else {
//                std::cerr << "[resolver] Not Found." << std::endl;
            }
        }
        return false;
    }
    
} /* http */ } /* serenity */


#endif /* end of include guard: SERENITY_HTTP_SERVICE_RESOLVER_HPP_ */
