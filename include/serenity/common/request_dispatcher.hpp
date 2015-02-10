#ifndef SERENITY_COMMON_REQUEST_DISPATCHER_HPP__
#define SERENITY_COMMON_REQUEST_DISPATCHER_HPP__


namespace serenity { namespace common {

    template <class service_resolver>
    class request_dispatcher {
        public:
            using request = typename service_resolver::request;
            using response = typename service_resolver::response;

            request_dispatcher(const service_resolver &);

            bool dispatch(const request &, response &);

        private:
            const service_resolver &service_resolver_;
    };

    template <class service_resolver>
    request_dispatcher<service_resolver>::request_dispatcher(const service_resolver &res) : service_resolver_(res) {
    }


    template <class service_resolver>
    bool request_dispatcher<service_resolver>::dispatch(const request &req, response &resp)
    {
        return false;
    }
    
} /* common */ } /* serenity */


#endif /* end of include guard: SERENITY_COMMON_REQUEST_DISPATCHER_HPP__ */
