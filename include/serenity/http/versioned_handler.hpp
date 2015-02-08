#ifndef SERENITY_HTTP_VERSIONED_HANDLER_HPP_
#define SERENITY_HTTP_VERSIONED_HANDLER_HPP_

#include "request_handler.hpp"

namespace serenity { namespace http {

    template <class handler_type>
    class versioned_handler : public request_handler<handler_type> {
        public:
            virtual request_status handle(const request &req, response &resp) override;

            uint32_t version = 0;
    };

    template <class handler_type>
    request_status versioned_handler<handler_type>::handle(const request &req, response &resp)
    {
        // URI: /v1/blah
        // split URI to grab version
        std::cerr << "[ver.handle] " << req.method << " -> " << req.uri << std::endl;
        std::string::size_type n = req.uri.find_first_of('/', 1);
        if (req.uri[0] != '/' || n == std::string::npos) {
            std::cerr << "[ver.handle] Version Error." << std::endl;
            return request_status::not_found;
        }
        else {
            std::string ver = req.uri.substr(1, n - 1);
            if (ver[0] == 'v') {
                ver.erase(0, 1);
                for (auto c : ver) {
                    if (c < '0' || c > '9')
                        return request_status::invalid_uri;
                }
                version = atoi(ver.c_str());
            }
            else {
                return request_status::invalid_uri;
            }
            request unver_req = req;
            unver_req.uri.erase(1, ver.length() + 2);
            return request_handler<handler_type>::handle(unver_req, resp);
        }
    }
    
} /* http */ } /* serenity */

#endif /* end of include guard: SERENITY_HTTP_VERSIONED_HANDLER_HPP_ */
