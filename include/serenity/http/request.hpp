#ifndef SERENITY_HTTP_REQUEST_HPP_
#define SERENITY_HTTP_REQUEST_HPP_
#include <string>
#include <map>
#include <iostream>

#include "service_resolver.hpp"
#include "response.hpp"

namespace serenity { namespace http {

    const unsigned int nothing = 0;
    const unsigned int crlf_01_start = 1;
    const unsigned int crlf_01_end = 2;
    const unsigned int crlf_02_start = 3;
    const unsigned int crlf_02_end = 4;
    const unsigned int post_end = 5;

    class request {
        public:
            /** \brief Method used for request: GET, PUT, DELETE, etc. */
            std::string method;

            /** \brief The requested URI, including parameters */
            std::string uri; // Convert to uri class from cpp-net

            /** \brief All of the HTTP headers provided by the client. */
            std::map<std::string, std::string> headers;

            /** \brief Major HTTP version (ie 1) */
            uint8_t version_major;

            /** \brief Minor HTTP version (ie 0) */
            uint8_t version_minor;

            /** \brief Raw bytes of the extra data provided by the client in the request */
            std::string post_data;

            /** \brief Add data to the request for parsing. */
            void add_data(char *request_data, std::size_t bytes) {
                std::cerr << "Data (" << bytes << "):" << std::endl
                    << request_data << std::endl;
                memcpy(data_.data() + data_end_, request_data, bytes);
                data_end_ += bytes;

                char *end_of_request = nullptr;
                int i = 0;
                for (; (i < bytes) && (parse_state_ < post_end); ++i) {
                    switch (parse_state_) {
                        case nothing:
                        case crlf_01_end:
                            if (request_data[i] == '\r') ++parse_state_;
                            else parse_state_ = nothing;
                            break;
                        case crlf_01_start:
                        case crlf_02_start:
                            if (request_data[i] == '\n') ++parse_state_;
                            else parse_state_ = nothing;
                            break;
                        case crlf_02_end: // TODO: This never gets hit.
                            end_of_request = &request_data[i];
                            ++parse_state_;
                            break;
                    }
                }
                is_complete_ = (parse_state_ == post_end);
                if (is_complete_)
                    parse();
            }

            bool is_complete() {
                return is_complete_;
            }

            bool is_error() {
                return is_error_;
            }

            /** \brief Parses the provided data as an HTTP request, and populates the current object. */
            bool parse() {
                // TODO: Error detection, do not support malformed requests.
                int state = 0;
                std::string header;
                std::string value;
                for (const char *p = data_.data(); (p - data_.data()) < data_end_; ++p) {
                    switch (state) {
                        case 0:
                            if (*p != ' ')
                                method = method + *p;
                            else
                                ++state;
                            break;
                        case 1:
                            if (*p != ' ')
                                uri = uri + *p;
                            else
                                ++state;
                            break;
                        case 2:
                            if (*p == '/')
                               ++state; 
                            break;
                        case 3:
                            if (*p != '.')
                                version_major = *p - '0'; // TODO: Support more than 1 digit values
                            else
                                ++state;
                            break;
                        case 4:
                            if (*p != '\r')
                                version_minor = *p - '0';
                            else
                                ++state;
                            break;
                        case 5:
                            if (*p == '\n') {
                                header = "";
                                value = "";
                                ++state;
                            }
                            else
                                return false;
                            break;
                        case 6: // Start Headers.
                            if (*p != ':' && *p != ' ')
                                header = header + *p;
                            else if (*p == ' ')
                                ++state;
                            break;
                        case 7:
                            if (*p != '\r' && *p != '\n')
                                value = value + *p;
                            else if (*p == '\n') {
                                headers.insert({ header, value });
                                ++state;
                            }
                            break;
                        case 8:
                            if (*p != '\r' && *p != '\n') {
                                header = *p;
                                value = "";
                                state = 6;
                            }
                            else if (*p == '\n') {
                                ++state;
                            }
                            break;
                        case 9:
                            if ((p - data_.data()) < data_end_) {
                                post_data =  std::string(p, data_end_ - (p - data_.data()));
                                ++state;
                            }
                            break;
                        default:
                            break;
                    }
                }
                return (state > 0);
            }

        private:
            std::array<char, 4096> data_;
            std::size_t data_end_;
            unsigned int parse_state_;
            bool is_complete_;
            bool is_error_;
    };
    
} /* http */ } /* serenity */

#endif /* end of include guard: SERENITY_HTTP_REQUEST_HPP_ */
