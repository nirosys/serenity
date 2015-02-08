#include <memory>
#include <iostream>
#include "boost/asio.hpp"

#include "protocol_handler.hpp"

#ifndef SERENITY_NET_CONNECTION_HPP_
#define SERENITY_NET_CONNECTION_HPP_

namespace serenity { namespace net {

    template <class req_handler>
    class connection_manager;

    /** \brief Handles data acquisition from the underlying socket.
     *
     *  Delegates the reading and writing of the socket, and handing it
     *  to the associated protocol_handler for processing.
     */
    template <class req_handler>
    class connection : public std::enable_shared_from_this<connection<req_handler>> {
        public:
            connection(const connection &) = delete;
            connection &operator=(const connection &) = delete;

            /** \brief Constructs a new connection, with the underlying socket
             *         and connection manager. */
            explicit connection(boost::asio::ip::tcp::socket socket,
                    connection_manager<req_handler> &manager);

            virtual ~connection() {}

            /** \brief Starts a new connection by wiring up the asio handlers */
            void start();

            /** \brief Stops a connection by closing the underlying socket. Does not
             *      remove the connection from the connection_manager. */
            void stop();

            /** \brief Removes connection from parent connection_manager. */
            void shutdown() { connection_manager_.remove(this->shared_from_this()); stop(); }

            /** \brief Returns the associated connection_manager */
            connection_manager<req_handler> &get_manager() { return connection_manager_; }

            using connection_ptr = std::shared_ptr<connection<req_handler>>;
        private:
            using proto_handler = typename req_handler::proto_handler;

            std::array<char, 1024> buffer_;
            boost::asio::ip::tcp::socket socket_;
            connection_manager<req_handler> &connection_manager_;
            proto_handler protocol_handler_;

            void do_read();
            void do_write(const typename proto_handler::response_buffer &);
    };

    #include "../../../src/net/connection.cc" 
} /* http */ } /* serenity */


#endif /* end of include guard: SERENITY_NET_CONNECTION_HPP_ */
