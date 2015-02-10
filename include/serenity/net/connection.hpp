#include <memory>
#include <iostream>
#include "boost/asio.hpp"

#include "serenity/common/request_dispatcher.hpp"

#ifndef SERENITY_NET_CONNECTION_HPP_
#define SERENITY_NET_CONNECTION_HPP_

namespace serenity { namespace net {

    template <class service_resolver_type>
    class connection_manager;

    /** \brief Handles data acquisition from the underlying socket.
     *
     *  Delegates the reading and writing of the socket, and handing it
     *  to the associated protocol_handler for processing.
     */
    template <class resolver_type>
    class connection : public std::enable_shared_from_this<connection<resolver_type>> {
        public:
            using service_resolver = resolver_type;
            using request = typename service_resolver::request;
            using response = typename service_resolver::response;
            using manager = connection_manager<resolver_type>;
            using dispatcher = common::request_dispatcher<resolver_type>;
            using response_buffer = boost::asio::const_buffer;
            using socket = boost::asio::ip::tcp::socket;

            connection(const connection &) = delete;
            connection &operator=(const connection &) = delete;

            /** \brief Constructs a new connection, with the underlying socket
             *         and connection manager. */
            connection(boost::asio::ip::tcp::socket socket,
                    manager &manager, dispatcher &dispatcher);

            virtual ~connection() {}

            /** \brief Starts a new connection by wiring up the asio handlers */
            void start();

            /** \brief Stops a connection by closing the underlying socket. Does not
             *      remove the connection from the connection_manager. */
            void stop();

            /** \brief Removes connection from parent connection_manager. */
            void shutdown() { connection_manager_.remove(this->shared_from_this()); stop(); }

            /** \brief Returns the associated connection_manager */
            manager &get_manager() { return connection_manager_; }

            using connection_ptr = std::shared_ptr<connection>;
        private:
            std::array<char, 1024> buffer_;
            boost::asio::ip::tcp::socket socket_;
            manager &connection_manager_;
            dispatcher &request_dispatcher_;
            response response_;
            request request_;

            void do_read();
            void do_write(const typename connection<resolver_type>::response_buffer &);
    };

    #include "../../../src/net/connection.cc" 
} /* http */ } /* serenity */


#endif /* end of include guard: SERENITY_NET_CONNECTION_HPP_ */
