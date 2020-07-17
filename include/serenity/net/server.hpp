#ifndef SERENITY_NET_SERVER_HPP
#define SERENITY_NET_SERVER_HPP

#include <functional>
#include <thread>
#include <condition_variable>
#include <memory>
#include <iostream>

#include <boost/thread.hpp>
#include <boost/asio.hpp>

#include "connection_manager.hpp"

const uint32_t max_io_service_threads = 32;

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
            using connection = net::connection<resolver>;
            using dispatcher = typename connection::dispatcher; 
            using manager = connection_manager<resolver>;

            server(const server &) = delete;

            /** \brief Create a new server listening on all interfaces on
             *         the provided port.
             */
            server(uint32_t port); // Default 0.0.0.0
            server(const std::string &address, uint32_t port);

            /** \brief Create a new server, listening on default port (8080) and
             *         all IPv4 addresses available.
             */
            server(); // Default port 8080, IP 0.0.0.0

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

            /** \brief Set the port that the server should listen on. (Default: 8080) */
            void set_port(uint16_t port) { port_ = port; }

            /** \brief Set the address to bind the server to. Can be IPv4 or IPv6. */
            void set_address(const std::string &address) { address_ = address; }

            /** \brief Returns the current service resolver being used by the server. */
            resolver &get_resolver() { return service_resolver_; }

        private:
            const uint16_t kDefaultPort = 8080;
            uint16_t port_ = kDefaultPort;
            std::string address_ = "0.0.0.0";
            boost::asio::io_service io_service_;
            boost::asio::ip::tcp::acceptor acceptor_;
            boost::asio::signal_set signals_;
            boost::thread_group running_threads_;
            std::mutex stop_mutex_;
            std::condition_variable stop_condition_;
            bool is_running_ = false;

            manager connection_manager_;
            resolver service_resolver_;
            dispatcher request_dispatcher_;

            // Sets up the stop handler to catch signals for shutdown cues.
            void do_wait_stop();

            // Handles incoming connections.
            void do_accept();
    };

    template <class resolver_type>
    server<resolver_type>::server() :
        io_service_(),
        signals_(io_service_),
        acceptor_(io_service_),
        service_resolver_(),
        request_dispatcher_(service_resolver_)
    {
        signals_.add(SIGINT);
        signals_.add(SIGTERM);
        signals_.add(SIGQUIT);

        do_wait_stop();
    }

    template <class resolver_type>
    server<resolver_type>::server(uint32_t port) : server()
    {
        port_ = port;
    }

    template <class resolver_type>
    server<resolver_type>::server(const std::string &address, uint32_t port)
       : server()
    {
        port_ = port;
        address_ = address;
    }

    template <class resolver_type>
    server<resolver_type>::~server() {
        if (is_running_) {
            stop();
        }
    }

    template <class resolver_type>
    void server<resolver_type>::run() {
        boost::asio::ip::address address = boost::asio::ip::address::from_string(address_);
        boost::asio::ip::tcp::resolver resolver(io_service_);
        boost::asio::ip::tcp::endpoint endpoint(address, port_);

        acceptor_.open(endpoint.protocol());
        acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
        acceptor_.bind(endpoint);
        acceptor_.listen();

        do_accept();

        running_threads_.join_all();

        is_running_ = true;
        for (int32_t x = 0; x < max_io_service_threads; x++) {
            running_threads_.create_thread(boost::bind(&boost::asio::io_service::run, &io_service_));
        }
    }

    template <class resolver_type>
    void server<resolver_type>::wait_to_end() {
        std::unique_lock<std::mutex> lk(stop_mutex_);
        stop_condition_.wait(lk, [this] { return !is_running_; });
        running_threads_.join_all();
    }

    template <class resolver_type>
    void server<resolver_type>::stop() {
        if (acceptor_.is_open())
            acceptor_.close();
        if (is_running_) {
            std::unique_lock<std::mutex> lk(stop_mutex_);
            io_service_.stop();
            running_threads_.join_all();
            stop_condition_.notify_one(); // Notify wait_to_end that we're done..
            is_running_ = false;
        }
    }

    template <class resolver_type>
    void server<resolver_type>::do_accept() {
        acceptor_.async_accept(io_service_,
                [this](const boost::system::error_code &ec, boost::asio::ip::tcp::socket socket) {
                    if (!acceptor_.is_open()) {
                        return;
                    }
                    if (!ec) {
                        // add connection to manager..
                        connection_manager_.start(
                            std::make_shared<connection>(
                                std::move(socket), 
                                connection_manager_, 
                                request_dispatcher_
                            )
                        );
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
