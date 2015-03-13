#ifndef SERENITY_HTTP_SERVICE_RESOLVER_HPP_
#define SERENITY_HTTP_SERVICE_RESOLVER_HPP_

#include <map>
#include "serenity/http/service.hpp"
#include "serenity/common/service_resolver.hpp"
#include "policies/url.hpp"

namespace serenity { namespace http {

    class service_not_found : public service {
        public:
           virtual bool handle(const request &, response &resp) override {
               resp.status = 404;
               resp.content = "";
               return true;
           } 
    };

    template <class request_type, class response_type, typename policy_type>
    class service_resolver {
        public:
            using request = request_type;
            using response = response_type;
            using service = http::service;
            using handler = typename common::service_resolver<request_type, response_type>::handler;
            service_resolver(const service_resolver &) = delete;
            service_resolver &operator=(const service_resolver &) = delete;

            service_resolver() {}

            bool resolve(request_type &, service *&);
            bool resolve(typename policy_type::key_type k, service *&);

            template <class service_type>
            service_type &add_service(typename policy_type::key_type key) {
                service_type *s = new service_type();
                services_[key] = std::unique_ptr<service>(s);
                return *s;
            }

            template <class service_type, typename... ctor_args>
            service_type &add_service(typename policy_type::key_type key, ctor_args... args) {
                service_type *s = new service_type(args...);
                services_[key] = std::unique_ptr<service>(s);
                return *s;
            }

        private:
            policy_type policies_;
            std::map<typename policy_type::key_type, std::unique_ptr<service>> services_;
            service_not_found not_found_service_;
    };

    template <class request_type, class response_type, typename policy_type>
    bool service_resolver<request_type, response_type, policy_type>::resolve(request_type &req, service *&svc)
    {
        std::string remaining;
        if (policies_.apply(req.uri, remaining)) {
            auto search = services_.find(policies_.values);
            if (search != services_.end()) {
                svc = &*(search->second);
                req.uri = remaining;
                return true;
            }
        }
        svc = &not_found_service_;
        return true;
    }

    template <class request_type, class response_type, typename policy_type>
    bool service_resolver<request_type, response_type, policy_type>::resolve(typename policy_type::key_type key, service *&svc)
    {
        auto search = services_.find(key);
        if (search != services_.end()) {
            svc = &*(search->second);
            return true;
        }
        return false;
    }
    
} /* http */ } /* serenity */


#endif /* end of include guard: SERENITY_HTTP_SERVICE_RESOLVER_HPP_ */
