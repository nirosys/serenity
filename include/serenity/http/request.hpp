#ifndef SERENITY_HTTP_REQUEST_HPP_
#define SERENITY_HTTP_REQUEST_HPP_
#include <string>
#include <map>
#include <iostream>

#include "response.hpp"

namespace serenity { namespace http {

    const unsigned int nothing = 0;
    const unsigned int crlf_01_start = 1;
    const unsigned int crlf_01_end = 2;
    const unsigned int crlf_02_start = 3;
    const unsigned int crlf_02_end = 4;
    const unsigned int post_end = 5;

    template <typename Enumeration>
    auto as_integer(Enumeration const value) -> typename std::underlying_type<Enumeration>::type
    {
        return static_cast<typename std::underlying_type<Enumeration>::type>(value);
    }

    class request {
        public:
            /** \brief Method used for request: GET, PUT, DELETE, etc. */
            std::string method;

            /** \brief The requested URI, including parameters */
            std::string uri; // Convert to uri class from cpp-net

            /** \brief All of the HTTP headers provided by the client. */
            std::map<std::string, std::string> headers;

            /** \brief All of the parameters supplied for the request. */
            std::map<std::string, std::string> parameters;

            /** \brief Major HTTP version (ie 1) */
            uint8_t version_major;

            /** \brief Minor HTTP version (ie 0) */
            uint8_t version_minor;

            /** \brief Raw bytes of the extra data provided by the client in the request */
            std::string post_data;

            /** \brief Add data to the request for parsing. */
            void add_data(const char *request_data, std::size_t bytes) {
                //std::cerr << "Data (" << bytes << "):" << std::endl
                //    << request_data << std::endl;
                memcpy(data_.data() + data_end_, request_data, bytes);
                data_end_ += bytes;

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
                        case crlf_02_end:
                            ++parse_state_;
                            break;
                    }
                }
                is_complete_ = (parse_state_ >= crlf_02_end);
                if (is_complete_) {
                    parse();
                }
            }

            bool is_complete() {
                return is_complete_;
            }

            bool is_error() {
                return is_error_;
            }

            /** \brief Parses the provided data as an HTTP request, and populates the current object. */
            bool parse() {
                parser_state state = parser_state::start;
                parser_state next_state = parser_state::start;
                char const *token_start = nullptr;
                std::string variable;
                std::string value;
                for (const char *p = data_.data(); (p - data_.data()) < data_end_; ++p) {
                    switch (state) {
                        case parser_state::start:
                            if (!set_error( (*p < 'A' || *p > 'Z') )) {
                                state = parser_state::method;
                                token_start = p;
                            }
                            break;
                        case parser_state::method:
                            if (!set_error( (*p < 'A' || *p > 'Z') && *p != ' ' )) {
                                if (*p == ' ') {
                                    method = std::string(token_start, p - token_start);
                                    state = parser_state::uri;
                                    token_start = p + 1; // +1 to move past ' '.
                                    //std::cerr << "[parse] Method: " << method << std::endl;
                                }
                            }
                            break;
                        case parser_state::uri:
                            if (!set_error( (*p < 32 || *p > 126) )) { // Accept all printables.
                                if (*p == '?') { // End of URI, start of params
                                    uri = decode_url(std::string(token_start, p - token_start));
                                    state = parser_state::uri_parameters;
                                    token_start = p + 1; // Move past '?'
                                    //std::cerr << "[parse] URI: " << uri << std::endl;
                                }
                                else if (*p == ' ') { // End of URI, start of version
                                    uri = decode_url(std::string(token_start, p - token_start));
                                    state = parser_state::http_version_HTTP;
                                    //std::cerr << "[parse] URI: " << uri << std::endl;
                                }
                            }
                            break;
                        case parser_state::uri_parameters:
                            if (*p == '=') {
                                variable = decode_url(std::string(token_start, p - token_start));
                                token_start = p + 1; // Move beyond '='
                            }
                            else if (*p == '&' || *p == ' ') {
                                value = decode_url(std::string(token_start, p - token_start));
                                if (variable.size() > 0)
                                    parameters[variable] = value;
                                else if (value.size() > 0)
                                    parameters[value] = "";
                                //std::cerr << "[parse] param: " << variable << " = " << value << std::endl;

                                if (*p == ' ')
                                    state = parser_state::http_version_HTTP;

                                token_start = p + 1; // Move beyond '&'
                            }
                            else if (*p == '\r' || *p == '\n')
                                state = parser_state::error;
                            break;
                        case parser_state::http_version_HTTP:
                            //std::cerr << "[parse] parsing HTTP: " << *p << std::endl;
                            // TODO: Proper HTTP sequence should be determined.
                            if (!set_error( (*p != 'H' && *p != 'T' && *p != 'P') && *p != '/')) {
                                if (*p == '/') {
                                    state = parser_state::http_version_major;
                                    token_start = p + 1; // +1 to move past '/'
                                }
                            }
                            break;
                        case parser_state::http_version_major:
                            //std::cerr << "[parse] parsing HTTP major: " << *p << std::endl;
                            if (!set_error( (*p < '0' || *p > '9') && (*p != '.') )) {
                                if ('.' == *p) {
                                    version_major = std::stol(std::string(token_start, p - token_start));
                                    state = parser_state::http_version_minor;
                                    token_start = p + 1; // +1 to move past '.'
                                }
                            }
                            break;
                        case parser_state::http_version_minor:
                            //std::cerr << "[parse] parsing HTTP minor: " << *p << std::endl;
                            if (!set_error( (*p < '0' || *p > '9') && (*p != '\r') )) {
                                if ('\r' == *p) {
                                    version_minor = std::stol(std::string(token_start, p - token_start));
                                    state = parser_state::header_name;
                                    token_start = p + 2; // Move past "\r\n"

                                    next_state = parser_state::header_name;
                                    state = parser_state::end_of_line;
                                }
                            }
                            break;
                        case parser_state::header_name:
                            if (*p == '\r') {
                                state = parser_state::end_of_line;
                                next_state = parser_state::post_data;
                            }
                            else if (!set_error(!identifier_char(*p))) {
                                if (*p == ':') {
                                    variable = std::string(token_start, p - token_start);
                                    token_start = p + 2; // Move past ": "
                                    state = parser_state::header_value;
                                }
                            }
                            break;
                        case parser_state::header_value:
                            if (*p == '\r') {
                                value = std::string(token_start, p - token_start);
                                next_state = parser_state::header_name;
                                state = parser_state::end_of_line;

                                token_start = p + 2; // Move past "\r\n"
                                headers[variable] = value;
                                //std::cerr << "[parse] " << variable << " = " << value << std::endl;
                            }
                            break;
                        case parser_state::end_of_line:
                            //std::cerr << "[parser] parsing end of line: " << *p << std::endl;
                            if (!set_error( *p != '\n' )) {
                                //std::cerr << "[parser] EOL parsed" << std::endl;
                                state = next_state;
                                token_start = p + 1; // Move past '\n'
                            }
                            break;
                        default:
                            break;
                    }
                    if (is_error_) {
                        //std::cerr << "[parse] ERROR - previous state: " << as_integer(state) << std::endl;
                        state = parser_state::error;
                        break;
                    }
                }
                if (state == parser_state::post_data) {
                    post_data = std::string(token_start, data_end_ - (token_start - data_.data()));
                }
                data_end_ = 0;
                return !is_error_;
            }

        private:
            enum class parser_state {
                start,
                method,
                uri,
                uri_parameters,
                http_version_HTTP,
                http_version_major,
                http_version_minor,
                header_name,
                header_value,
                end_of_request_maybe,
                post_data,
                error,
                end_of_line
            };
            std::array<char, 4096> data_;
            std::size_t data_end_ = 0;
            unsigned int parse_state_;
            bool is_complete_;
            bool is_error_;

            inline bool set_error(bool is_error) { return (is_error_ = is_error); }
            bool identifier_char(char c) {
                std::string valid_chars = "abcdefghijklmnopqrstuvwxyz0123456789-:";
                auto p = valid_chars.find_first_of(::tolower(c));
                return (p != std::string::npos);
            }

            std::string decode_url(const std::string &str) {
                char hex[3] = { 0, 0, 0 };
                bool in_hex = false;
                std::string decoded = "";
                for (int i=0; i < str.size(); ++i) {
                    if (in_hex) {
                        if (hex[0] == '\0')
                            hex[0] = str[i];
                        else {
                            hex[1] = str[i];
                            in_hex = false;
                            char c = std::strtol(hex, nullptr, 16);
                            decoded += c;
                        }
                    }
                    else if (str[i] != '%')
                        decoded += str[i];
                    else
                        in_hex = true;
                }
                return decoded;
            }
    };
    
} /* http */ } /* serenity */

#endif /* end of include guard: SERENITY_HTTP_REQUEST_HPP_ */
