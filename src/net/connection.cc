template <class request_type, class response_type>
connection<request_type, response_type>::connection(connection::socket socket, connection_manager<request_type, response_type> &manager) :
    socket_(std::move(socket)),
    connection_manager_(manager)
{
}

template <class request_type, class response_type>
void connection<request_type, response_type>::start() {
    do_read();
}

template <class request_type, class response_type>
void connection<request_type, response_type>::stop() {
    socket_.close();
}

template <class request_type, class response_type>
void connection<request_type, response_type>::do_read() {
    auto self(this->shared_from_this());
    socket_.async_read_some(boost::asio::buffer(buffer_),
            [this, self](boost::system::error_code ec, std::size_t bytes)
            {
                if (!ec) {
                    // TODO: Implement request parsing, and dispatch.
                    //protocol_status status = protocol_handler_.process(buffer_.data(), bytes);
                    //if (status == protocol_status::response) {
                    //   do_write(protocol_handler_.get_response());
                    //   std::cerr << "==========================================" << std::endl;
                    //}
                    //else if (status == protocol_status::error) {
                    //   std::cerr << "Error received from proto_handler." << std::endl;
                    //   shutdown();
                    //   return;
                    //}
                }
                else if (ec != boost::asio::error::operation_aborted) {
                    shutdown();
                    return;
                }

                do_read();
            }
    );
}

template <class request_type, class response_type>
void connection<request_type, response_type>::do_write(const typename connection::response_buffer &buffer) {
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
