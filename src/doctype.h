#ifndef COW_DOCTYPES_H
#define COW_DOCTYPES_H
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>

namespace core {
struct Response {
  std::string status, message, body, version, path;
  std::unordered_map<std::string, std::string> headers;
  int socket;
  int send() const;
  int redirect(const std::string &path); // TODO: IMPL
  int sendFile(const std::string &path, const std::string &mimetype); // TODO: IMPL
};
struct Request {
  std::string method, path, version;
  std::unordered_map<std::string, std::string> headers;
};

}; // namespace core
struct HTML {
  std::string filename;
  HTML(const std::string &filename, core::Response *res)
      : filename("public/" + filename) {
   std::string contents;
    {
      if (!res->body.empty()) {
        contents = res->body;
        fprintf(stdout,
                "[WARNING] The response body is NOT empty for [%s], fill be "
                "using the response body not the HTML file!\n",
                res->path.c_str());
      } else {
        std::ifstream file(filename);
        if (!file.is_open()) {
          fprintf(stderr, "File not found: %s\n", filename.c_str());
          exit(1);
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        contents = buffer.str();
        file.close();
      }
    } // EOS
    res->body = contents;
    res->version = "HTTP/1.1";
    res->status = "200";
    res->message = "OK";
    res->headers["Content-Type"] = "text/html";
  }
};

#endif // !#ifndef COW_DOCTYPES_H
