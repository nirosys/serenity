#ifndef SERENITY_COMMON_SERVICE_RESOLVER_HPP__
#define SERENITY_COMMON_SERVICE_RESOLVER_HPP__

namespace serenity { namespace common {

    template <class request_type, class response_type>
    class service_resolver {
        public:
            using request = request_type;
            using response = response_type;
            using handler = std::function<uint32_t(const request_type &, response_type &)>;
            service_resolver(const service_resolver &) = delete;
            service_resolver &operator=(const service_resolver &) = delete;

            virtual handler resolve(const request_type &) = 0;

        protected:
            service_resolver() { }
    };
    
} /* common */ } /* serenity */



#endif /* end of include guard: SERENITY_COMMON_SERVICE_RESOLVER_HPP__ */
