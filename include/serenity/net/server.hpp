#ifndef SERENITY_NET_SERVER_HPP
#define SERENITY_NET_SERVER_HPP

#include <functional>
#include <boost/asio.hpp>
#include <thread>
#include <memory>
#include <iostream>

#include "connection_manager.hpp"

namespace serenity { namespace net {

    /** \brief TCP Server for accepting connections, and delegating handlers.
     *
     *  Provides the main entry point for serenity by creating a listening
     *  TCP socket on the provided port/address and accepting incoming connections.
     */
    template <class resolver_type>
    class server {
        public:
            using resolver = resolver_type;
            using request = typename resolver_type::request;
            using response = typename resolver_type::request;
            using connection = connection<resolver>;
            using dispatcher = typename connection::dispatcher; 
            using manager = connection_manager<resolver>;

            server(const server &) = delete;
            server(void) = delete;

            /** \brief Create a new server listening on all interfaces on
             *         the provided port.
             */
            server(uint32_t port); // Default 0.0.0.0

            ~server();

            
            /** \brief Returns true if the server has successfully started.
             *         False otherwise.
             */
            bool is_running() const { return is_running_; }

            /** \brief Starts the current server. */
            void run();

            /** \brief Stops the current server, severing all connections. */
            void stop();

            /** \brief Blocks thread execution until the server has successfully
             *         shut down. */
            void wait_to_end();


            resolver &get_resolver() { return service_resolver_; }

        private:
            boost::asio::io_service io_service_;
            boost::asio::ip::tcp::acceptor acceptor_;
            boost::asio::ip::tcp::socket socket_;
            boost::asio::signal_set signals_;
            std::thread running_thread_;
            bool is_running_;

            manager connection_manager_;
            resolver service_resolver_;
            dispatcher request_dispatcher_;

            // Sets up the stop handler to catch signals for shutdown cues.
            void do_wait_stop();

            // Handles incoming connections.
            void do_accept();
    };

    template <class resolver_type>
    server<resolver_type>::server(uint32_t port) :
        io_service_(),
        signals_(io_service_),
        acceptor_(io_service_),
        socket_(io_service_),
        service_resolver_(),
        request_dispatcher_(service_resolver_)
    {
        signals_.add(SIGINT);
        signals_.add(SIGTERM);
        signals_.add(SIGQUIT);

        do_wait_stop();

        boost::asio::ip::tcp::resolver resolver(io_service_);
        boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), port);
        acceptor_.open(endpoint.protocol());
        acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
        acceptor_.bind(endpoint);
        acceptor_.listen();

        do_accept();
    }

    template <class resolver_type>
    server<resolver_type>::~server() {
        if (is_running_) {
            stop();
        }
    }

    template <class resolver_type>
    void server<resolver_type>::run() {
        is_running_ = true;
        running_thread_ = std::thread(
                [this]() {
                    io_service_.run();
                }
        );
    }

    template <class resolver_type>
    void server<resolver_type>::wait_to_end() {
        if (running_thread_.joinable())
            running_thread_.join();
    }

    template <class resolver_type>
    void server<resolver_type>::stop() {
        acceptor_.close();
        if (is_running_) {
            io_service_.stop();
            if (running_thread_.joinable())
                running_thread_.join();
            is_running_ = false;
        }
    }

    template <class resolver_type>
    void server<resolver_type>::do_accept() {
        acceptor_.async_accept(socket_,
                [this](boost::system::error_code ec)
                {
                    if (!acceptor_.is_open()) {
                        return;
                    }
                    if (!ec) {
                        // add connection to manager..
                        connection_manager_.start(
                                std::make_shared<connection>(
                                    std::move(socket_),
                                    connection_manager_,
                                    request_dispatcher_
                                )
                        );
                        socket_.close();
                    }

                    do_accept();
                }
        );
    }

    template <class resolver_type>
    void server<resolver_type>::do_wait_stop() {
        signals_.async_wait(
                [this](boost::system::error_code, int)
                {
                    stop();
                    connection_manager_.stop();
                }
        );
    }
} }

#endif /* end of include guard: SERENITY_SERVER_HPP */
