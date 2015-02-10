template <class service_resolver_type>
connection<service_resolver_type>::connection(connection::socket socket, manager &manager, dispatcher &dispatcher) :
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
                        response resp;
                        if (request_dispatcher_.dispatch(request_, resp)) {
                           do_write(resp.to_buffers());
                           std::cerr << "Finished request successfully." << std::endl;
                        }
                        else {
                           std::cerr << "Finished request in error." << std::endl;
                           shutdown();
                           return;
                        }
                    }
                    else if (request_.is_error()) {
                        std::cerr << "Closing connection due to error." << std::endl;
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
                if (ec != boost::asio::error::operation_aborted) {
                    connection_manager_.stop(this->shared_from_this());
                }
            }
    );
}
