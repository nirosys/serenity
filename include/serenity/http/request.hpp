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
    const uint32_t initial_data_sz = 128000;

    template <typename Enumeration>
    auto as_integer(Enumeration const value) -> typename std::underlying_type<Enumeration>::type
    {
        return static_cast<typename std::underlying_type<Enumeration>::type>(value);
    }

    class request {
        public:
            request() { data_ = (char *)malloc(initial_data_sz); }
            ~request() { if (data_) { free(data_); data_ = nullptr; } }

            request(const request &r) {
                data_ = nullptr;
                data_ptr_ = data_;
                post_data_start_offset_ = 0;
                data_end_ = 0;
                header_length_ = 0;
                data_sz_ = initial_data_sz;
                parse_state_ = 0;
                is_complete_ = false;
                headers_complete_ = false;
                is_error_ = false;
                parser_state_ = parser_state::start;

                method = r.method;
                uri = r.uri;
                function = r.function;
                extra_path = r.extra_path;
                headers = r.headers;
                parameters = r.parameters;
                version_major = r.version_major;
                version_minor = r.version_minor;
                post_data = r.post_data;
            }

            /** \brief Method used for request: GET, PUT, DELETE, etc. */
            std::string method = "";

            /** \brief The requested URI, including parameters */
            std::string uri = ""; // Convert to uri class from cpp-net

            /** \brief The requested function name */
            std::string function = ""; // First token after the service resolution info.

            /** \brief Extra URI arguments, path elements after the first token. */
            std::string extra_path = "";

            /** \brief All of the HTTP headers provided by the client. */
            std::map<std::string, std::string> headers;

            /** \brief All of the parameters supplied for the request. */
            std::map<std::string, std::string> parameters;

            /** \brief Major HTTP version (ie 1) */
            uint8_t version_major = 0;

            /** \brief Minor HTTP version (ie 0) */
            uint8_t version_minor = 0;

            /** \brief Raw bytes of the extra data provided by the client in the request */
            std::string post_data = "";

            /** \brief Add data to the request for parsing. */
            void add_data(const char *request_data, std::size_t bytes) {
                //std::cerr << "Data (" << bytes << "):" << std::endl
                //    << request_data << std::endl;
                if ((data_end_ + bytes) > data_sz_) {
                    data_ = (char *)realloc(data_, data_end_ + bytes + 1);
                }
                memcpy(data_ + data_end_, request_data, bytes);
                data_end_ += bytes;
                data_ptr_ = (data_ + data_end_) - bytes;

                if (headers_complete_)  {
                    // If we're done with the headers, go straight to parse()
                    parse();
                    return;
                }

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
                headers_complete_ = (parse_state_ >= crlf_02_end);
                if (headers_complete_) {
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
                parser_state next_state = parser_state::start;
                char const *token_start = nullptr;
                std::string variable;
                std::string value;
                for (const char *p = data_ptr_; (p - data_) < data_end_; ++p) {
                    switch (parser_state_) {
                        case parser_state::start:
                            if (!set_error( (*p < 'A' || *p > 'Z') )) {
                                parser_state_ = parser_state::method;
                                token_start = p;
                            }
                            break;
                        case parser_state::method:
                            if (!set_error( (*p < 'A' || *p > 'Z') && *p != ' ' )) {
                                if (*p == ' ') {
                                    method = std::string(token_start, p - token_start);
                                    parser_state_ = parser_state::uri;
                                    token_start = p + 1; // +1 to move past ' '.
                                    //std::cerr << "[parse] Method: " << method << std::endl;
                                }
                            }
                            break;
                        case parser_state::uri:
                            if (!set_error( (*p < 32 || *p > 126) )) { // Accept all printables.
                                if (*p == '?') { // End of URI, start of params
                                    uri = decode_url(std::string(token_start, p - token_start));
                                    parser_state_ = parser_state::uri_parameters;
                                    token_start = p + 1; // Move past '?'
                                    //std::cerr << "[parse] URI: " << uri << std::endl;
                                }
                                else if (*p == ' ') { // End of URI, start of version
                                    uri = decode_url(std::string(token_start, p - token_start));
                                    parser_state_ = parser_state::http_version_HTTP;
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
                                    parser_state_ = parser_state::http_version_HTTP;

                                token_start = p + 1; // Move beyond '&'
                            }
                            else if (*p == '\r' || *p == '\n')
                                parser_state_ = parser_state::error;
                            break;
                        case parser_state::http_version_HTTP:
                            //std::cerr << "[parse] parsing HTTP: " << *p << std::endl;
                            // TODO: Proper HTTP sequence should be determined.
                            if (!set_error( (*p != 'H' && *p != 'T' && *p != 'P') && *p != '/')) {
                                if (*p == '/') {
                                    parser_state_ = parser_state::http_version_major;
                                    token_start = p + 1; // +1 to move past '/'
                                }
                            }
                            break;
                        case parser_state::http_version_major:
                            //std::cerr << "[parse] parsing HTTP major: " << *p << std::endl;
                            if (!set_error( (*p < '0' || *p > '9') && (*p != '.') )) {
                                if ('.' == *p) {
                                    version_major = std::stol(std::string(token_start, p - token_start));
                                    parser_state_ = parser_state::http_version_minor;
                                    token_start = p + 1; // +1 to move past '.'
                                }
                            }
                            break;
                        case parser_state::http_version_minor:
                            //std::cerr << "[parse] parsing HTTP minor: " << *p << std::endl;
                            if (!set_error( (*p < '0' || *p > '9') && (*p != '\r') )) {
                                if ('\r' == *p) {
                                    version_minor = std::stol(std::string(token_start, p - token_start));
                                    token_start = p + 2; // Move past "\r\n"

                                    next_state = parser_state::header_name;
                                    parser_state_ = parser_state::end_of_line;
                                }
                            }
                            break;
                        case parser_state::header_name:
                            if (*p == '\r') {
                                parser_state_ = parser_state::end_of_line;
                                next_state = parser_state::post_data;
                                //post_data_start_ptr_ = p + 2; // move past \r\n
                                post_data_start_offset_ = (p - data_) + 2;
                                post_data.clear();
                                header_length_ = &data_[post_data_start_offset_] - data_ptr_;
                            }
                            else if (!set_error(!identifier_char(*p))) {
                                if (*p == ':') {
                                    variable = std::string(token_start, p - token_start);
                                    token_start = p + 2; // Move past ": "
                                    parser_state_ = parser_state::header_value;
                                }
                            }
                            break;
                        case parser_state::header_value:
                            if (*p == '\r') {
                                value = std::string(token_start, p - token_start);
                                next_state = parser_state::header_name;
                                parser_state_ = parser_state::end_of_line;

                                token_start = p + 2; // Move past "\r\n"
                                headers[variable] = value;
                                //std::cerr << "[parse] " << variable << " = " << value << std::endl;
                            }
                            break;
                        case parser_state::end_of_line:
                            //std::cerr << "[parser] parsing end of line: " << *p << std::endl;
                            if (!set_error( *p != '\n' )) {
                                //std::cerr << "[parser] EOL parsed" << std::endl;
                                parser_state_ = next_state;
                                token_start = p + 1; // Move past '\n'
                            }
                            break;
                        default:
                            break;
                    }
                    if (is_error_) {
                        //std::cerr << "[parse] ERROR - previous state: " << as_integer(state) << std::endl;
                        parser_state_ = parser_state::error;
                        break;
                    }
                }
                if (!is_complete_ && parser_state_ == parser_state::post_data && 
                        headers.find("Content-Length") != headers.end()) {
                    // This variable is for debugging puposes.
                    uint32_t content_length = 
                        strtoul(headers["Content-Length"].c_str(), NULL, 0);
                    if (data_end_ - header_length_ >= content_length) {
                        post_data = std::string(&data_[post_data_start_offset_], content_length);
                        is_complete_ = true;
                    }
                }
                else {
                    is_complete_ = true;
                    data_end_ = 0;
                }

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

            // TODO:  Make this dynamic without the potential for uploading
            // huge files.
            //std::array<char, 2000000> data_;
            char *data_ = nullptr;
            char *data_ptr_ = data_;
            //const char *post_data_start_ptr_ = nullptr;
            std::size_t post_data_start_offset_ = 0;
            std::size_t data_end_ = 0;
            std::size_t header_length_ = 0;
            std::size_t data_sz_ = initial_data_sz;
            unsigned int parse_state_ = 0;
            bool is_complete_ = false;
            bool headers_complete_ = false;
            bool is_error_ = false;
            parser_state parser_state_ = parser_state::start;

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

            void operator =(const request &o) = delete;
    };
    
} /* http */ } /* serenity */

#endif /* end of include guard: SERENITY_HTTP_REQUEST_HPP_ */
