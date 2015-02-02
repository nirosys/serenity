#ifndef SERENITY_NET_SERVER_HPP
#define SERENITY_NET_SERVER_HPP

#include <functional>
#include <boost/asio.hpp>
#include <thread>
#include <memory>
#include <iostream>

#include "connection_manager.hpp"

namespace serenity { namespace net {

    template <class req_handler>
    class server {
        public:
            server(const server &) = delete;
            server(void) = delete;

            server(uint32_t port); // Default 0.0.0.0

            ~server();

            // Status
            bool is_running() const { std::cerr << "Running: " << is_running_ << std::endl; return is_running_; }

            void run();
            void stop();


        private:
            boost::asio::io_service io_service_;
            boost::asio::ip::tcp::acceptor acceptor_;
            boost::asio::ip::tcp::socket socket_;
            boost::asio::signal_set signals_;
            std::thread running_thread_;
            bool is_running_;
            connection_manager<req_handler> connection_manager_;

            void do_wait_stop();
            void do_accept();
    };

    template <class req_handler>
    server<req_handler>::server(uint32_t port) :
        io_service_(),
        signals_(io_service_),
        acceptor_(io_service_),
        socket_(io_service_)
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

    template <class req_handler>
    server<req_handler>::~server() {
        if (is_running_) {
            stop();
        }
    }

    template <class req_handler>
    void server<req_handler>::run() {
        running_thread_ = std::thread(
                [this]() {
                    is_running_ = true;
                    io_service_.run();
                }
        );
    }

    template <class req_handler>
    void server<req_handler>::stop() {
        acceptor_.close();
        if (is_running_) {
            io_service_.stop();
            if (running_thread_.joinable())
                running_thread_.join();
            is_running_ = false;
        }
    }

    template <class req_handler>
    void server<req_handler>::do_accept() {
        acceptor_.async_accept(socket_,
                [this](boost::system::error_code ec)
                {
                    if (!acceptor_.is_open()) {
                        return;
                    }
                    if (!ec) {
                        // add connection to manager..
                        connection_manager_.start(std::make_shared<connection<req_handler>>(
                                    std::move(socket_), connection_manager_)
                        );
                        socket_.close();
                    }

                    do_accept();
                }
        );
    }

    template <class req_handler>
    void server<req_handler>::do_wait_stop() {
        signals_.async_wait(
                [this](boost::system::error_code, int)
                {
                    stop();
                    connection_manager_.stop();
                    //connection_manager_.start(std::make_shared<connection>(
                    //            std::move(socket_), connection_manager_, 
                    // Connection manager.. close all connections
                }
        );
    }
} }

#endif /* end of include guard: SERENITY_SERVER_HPP */
