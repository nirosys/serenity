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
            connection(boost::asio::ip::tcp::socket socket, manager &manager, dispatcher &dispatcher);

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

    template <class service_resolver_type>
    connection<service_resolver_type>::connection(boost::asio::ip::tcp::socket socket, 
                                                  manager &manager, dispatcher &dispatcher) :
               socket_(std::move(socket)),
               connection_manager_(manager),
               request_dispatcher_(dispatcher)
    {
    }

    template <class service_resolver_type>
    void connection<service_resolver_type>::start() {
        do_read();
    }

    template <class service_resolver_type>
    void connection<service_resolver_type>::stop() {
        socket_.close();
    }

    template <class service_resolver_type>
    void connection<service_resolver_type>::do_read() {
        auto self(this->shared_from_this());
        socket_.async_read_some(boost::asio::buffer(buffer_),
                [this, self](boost::system::error_code ec, std::size_t bytes)
                {
                    if (!ec) {
                        request_.add_data(buffer_.data(), bytes);
                        if (request_.is_complete()) {
                            if (request_dispatcher_.dispatch(request_, response_)) {
                                do_write(response_.to_buffers());
                            //std::cerr << "Finished request successfully." << std::endl;
                            }
                            else {
                                //std::cerr << "Finished request in error." << std::endl;
                                shutdown();
                                return;
                            }
                        }
                        else if (request_.is_error()) {
                            //std::cerr << "Closing connection due to error." << std::endl;
                            shutdown();
                            return;
                        }
                    }
                    else if (ec != boost::asio::error::operation_aborted) {
                        shutdown();
                        return;
                    }
                    do_read();
                }
        );
    }

    template <class service_resolver_type>
    void connection<service_resolver_type>::do_write(const typename connection::response_buffer &buffer) {
        auto self(this->shared_from_this());
        std::array<boost::asio::const_buffer, 1> buffers = {{ buffer }};
        boost::asio::async_write(socket_, buffers,
                [this, self](boost::system::error_code ec, std::size_t size)
                {
                    if (ec) {
                        std::cerr << "Error writing data: " <<  ec.message() << std::endl;
                        if (ec != boost::asio::error::operation_aborted)
                            connection_manager_.stop(this->shared_from_this());
                    }
                }
        );
    }
} /* http */ } /* serenity */

#endif /* end of include guard: SERENITY_NET_CONNECTION_HPP_ */
