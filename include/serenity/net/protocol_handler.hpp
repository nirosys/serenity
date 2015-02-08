#ifndef SERENITY_NET_PROTOCOL_HANDLER_HPP_
#define SERENITY_NET_PROTOCOL_HANDLER_HPP_

namespace serenity { namespace net {

    /** \brief Status descriptions for potential protocol_handler states. */
    enum class protocol_status {
        no_response = 0,
        response,
        error
    };

    /** \brief Parent class for protocol_handler implementations. */
    class protocol_handler {
        public:
            /** \brief Entry point for protocol handlers, provides handler with
             *         data read from socket for parsing. */
            virtual protocol_status process(char *data, std::size_t bytes) = 0;
    };
    
} /* net */ } /* serenity */


#endif /* end of include guard: SERENITY_NET_PROTOCOL_HANDLER_HPP_ */
