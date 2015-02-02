#ifndef SERENITY_NET_PROTOCOL_HANDLER_HPP_
#define SERENITY_NET_PROTOCOL_HANDLER_HPP_

namespace serenity { namespace net {

    class protocol_handler {
        public:
            enum class status {
                no_response = 0,
                response,
                error
            };
    };
    
} /* net */ } /* serenity */


#endif /* end of include guard: SERENITY_NET_PROTOCOL_HANDLER_HPP_ */
