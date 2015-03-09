#include "serenity/common/service.hpp"
#include "request.hpp"

#ifndef SERENITY_HTTP_SERVICE_HPP__
#define SERENITY_HTTP_SERVICE_HPP__

namespace serenity { namespace http {

    class response;

    class service : public common::service<request, response> {
        public:
            using handler_type = std::function<uint32_t(const request&, response&)>;

            // TODO: DFA based lookup. This would allow for easy matching of 'function's and
            //   allow for extra_path to be populated even if the function name is a multi-token
            //   path element.
            virtual bool handle(const request &req, response &resp) override {
                std::string method = req.method; // TODO: Make sure upcase.
                std::string uri = req.uri;

                if (uri[0] == '/') uri = uri.substr(1);

                std::string::size_type extra_start = uri.find_first_of('/');
                std::string function = uri.substr(0, extra_start);

                handler_key key = {method, function};
                request req_tmp = req;
                req_tmp.function = function;
                if (extra_start != std::string::npos)
                    req_tmp.extra_path = uri.substr(extra_start);

                auto search = handlers_.find(key);
                if (search != handlers_.end()) {
                    try {
                        uint32_t ret = search->second(req_tmp, resp); // TODO: Do something with the return.
                    }
                    catch (std::exception &e) {
                        std::cerr << "Exception while running handler: " << method << "/" << uri << std::endl
                                  << e.what() << std::endl;
                        resp.status = 500;
                        resp.content = "";
                    }
                    return true;
                }
                resp.status = 404;
                resp.content = "";
                return true;
            }

            void add(const std::string &method, const std::string &path, handler_type handler)
            {
                std::string up_method = method;
                std::transform(method.begin(), method.end(), up_method.begin(), ::toupper);

                handler_key key = {up_method, path};
                handlers_[key] = handler;
                //        std::cerr << "Added " << up_method << " handler for: " << path << std::endl;
            }

            void add_get(const std::string &path, handler_type handler) {
                add("GET", path, handler);
            }

            void add_put(const std::string &path, handler_type handler) {
                add("PUT", path, handler);
            }

            void add_post(const std::string &path, handler_type handler) {
                add("POST", path, handler);
            }

            void add_delete(const std::string &path, handler_type handler) {
                add("DELETE", path, handler);
            }
        private:
            using handler_key = std::pair<std::string, std::string>; // Method, Path
            std::map<handler_key, handler_type> handlers_;
    };

    
} /* http */ } /* serenity */


#endif /* end of include guard: SERENITY_HTTP_SERVICE_HPP__ */
