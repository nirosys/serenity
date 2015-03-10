#include "service.hpp"
#include "boost/filesystem.hpp"
#include <fstream>
#define SERENITY_DEBUG
//#include "serenity/log.hpp"

#ifndef SERENITY_HTTP_FILE_SERVICE_HPP__
#define SERENITY_HTTP_FILE_SERVICE_HPP__

namespace serenity { namespace http {

    class file_service : public service {
        public:
            file_service(const std::string &docroot) : root_path_(docroot) {}

            virtual bool handle(const request &req, response &resp) override {
                if (req.method != "GET") {
                    resp.status = 400;
                    resp.content = "";
                    return true;
                }

                std::string file_path = req.uri;
                if (file_path != "") {
                    boost::system::error_code ec;
                    std::string canon_root = boost::filesystem::canonical(root_path_, ec).native();
                    boost::filesystem::path path = boost::filesystem::canonical(root_path_ + "/" + file_path, ec);
                    if (!ec) {
                        std::string path_str = path.native();
                        if (path_str.compare(0, canon_root.size(), canon_root) == 0) {
                            uintmax_t size = boost::filesystem::file_size(path);
                            std::ifstream is(path_str, std::ifstream::binary);
                            if (is) {
                                resp.status = 200;
                                std::string type = file_content_type(file_path);
                                resp.headers.push_back({"Content-type", type});
                                resp.headers.push_back({"Content-length", std::to_string(size)});
                                resp.content.resize(size, ' ');
                                char *begin = &*resp.content.begin();
                                is.read(begin, size);
                                is.close();
                            }
                            else {
                                resp.status = 404; // TODO: Change to auth denied.
                                resp.content = "";
                            }

                            return true;
                        }
                    }
                }
                resp.status = 404;
                resp.content = "";
                return true;
            }

        protected:
            virtual std::string file_content_type(const std::string &filename) {
                auto dot = filename.find_last_of('.');
                if (dot != std::string::npos) {
                    std::string ext = filename.substr(dot);
                    if (ext == ".html")
                        return "text/html";
                    else if (ext == ".json")
                        return "application/json";
                    else if (ext == ".js")
                        return "text/javascript";
                    else if (ext == ".png")
                        return "image/png";
                    // TODO: Add MIME types.
                }
                return "application/octet-stream";
            }

        private:
            std::string root_path_;
    };

} }

#endif /* end of include guard: SERENITY_HTTP_FILE_SERVICE_HPP__ */
