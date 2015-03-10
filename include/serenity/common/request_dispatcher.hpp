#ifndef SERENITY_COMMON_REQUEST_DISPATCHER_HPP__
#define SERENITY_COMMON_REQUEST_DISPATCHER_HPP__

namespace serenity { namespace common {

    template <class service_resolver>
    class request_dispatcher {
        public:
            using request = typename service_resolver::request;
            using response = typename service_resolver::response;
            using handler = typename service_resolver::handler;

            request_dispatcher(service_resolver &);

            bool dispatch(const request &, response &);

        private:
            service_resolver &service_resolver_;
    };

    template <class service_resolver>
    request_dispatcher<service_resolver>::request_dispatcher(service_resolver &res) :
        service_resolver_(res)
    {
    }

    template <class service_resolver>
    bool request_dispatcher<service_resolver>::dispatch(const request &req_const, response &resp)
    {
        // NOTE: if the request does not specify a valid end-point then the
        //  resolver should return a handler that will generate a NOT FOUND,
        //  or similar response. Unless, it is desired that the connection be
        //  severed immediately.

        typename service_resolver::service *svc;

        request req = req_const;

        if (service_resolver_.resolve(req, svc)) {
            if (svc->handle(req, resp)) {
                return true;
            }
        }
        return false;
    }
    
} /* common */ } /* serenity */

#endif /* end of include guard: SERENITY_COMMON_REQUEST_DISPATCHER_HPP__ */
