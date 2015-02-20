#ifndef SERENITY_COMMON_SERVICE_RESOLVER_HPP__
#define SERENITY_COMMON_SERVICE_RESOLVER_HPP__
#include <memory>
#include <vector>
#include "service.hpp"

namespace serenity { namespace common {

    template <class request_type, class response_type>
    class service_resolver {
        public:
            using request = request_type;
            using response = response_type;
            using handler = std::function<uint32_t(const request_type &, response_type &)>;
            service_resolver(const service_resolver &) = delete;
            service_resolver &operator=(const service_resolver &) = delete;

            bool resolve(const request_type &, service<request_type, response_type> &) {
                return false;
            }

            // TODO: Add constructor params.
            template <class service_type>
            void add_service(const std::string &name) {
                services_[name] = std::unique_ptr<service<request_type,response_type>>(new service_type());
                std::cerr << "[resolver] Adding '" << name << "' of type: " << typeid(service_type).name() << std::endl;
            }

        protected:
            service_resolver() { }
            std::map<std::string, std::unique_ptr<service<request_type, response_type>>> services_;
    };
    
} /* common */ } /* serenity */


#endif /* end of include guard: SERENITY_COMMON_SERVICE_RESOLVER_HPP__ */
