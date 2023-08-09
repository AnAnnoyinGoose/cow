#ifndef COW_DOCTYPES_H
#define COW_DOCTYPES_H
#include <cstddef>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
namespace core
{
  struct Response
  {
    std::string status, message, body, version, path;
    std::unordered_map<std::string, std::string> headers;
    int socket;
    int send() const;
    int redirect(const std::string &path);
    template <class T> int sendFile(const T *filetype); 
    void pnf(std::string contents);
  };
  struct Request
  {
    std::string method, path, version;
    std::unordered_map<std::string, std::string> headers;
  };

}; // namespace core
struct HTML
{
  std::string filename, contents;
  std::unordered_map<const char *, const char *> data;
  HTML(const std::string &filename,
       std::unordered_map<const char *, const char *> data)
      : data(data)
  {
    this->filename = "public/" + filename;
    std::ifstream file(this->filename);
    if (!file.is_open())
    {
      fprintf(stderr, "File not found: %s\n", this->filename.c_str());
      exit(1);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    contents = buffer.str();
    file.close();
  }
  void replace(const std::string &s1, const std::string &s2)
  {
    const std::string key = "{{ " + s1 + " }}";
    size_t i = contents.find(key);
    if (i != std::string::npos)
    {
      contents.replace(i, key.length(), s2);
    }
  }
  core::Response send()
  {
    for (auto &data : this->data)
    {
      this->replace(data.first, data.second);
    }
    core::Response res{};
    res.headers["Content-Length"] = std::to_string(contents.size());
    res.headers["Content-Type"] = "text/html";
    res.body = contents;
    res.version = "HTTP/1.1";
    res.status = "200";
    res.message = "OK";
    return res;
  }
};
struct IMAGE
{
  std::string filename, contents;
  IMAGE(const std::string &filename, core::Response *res)
      : filename("public/assets/img/" + filename)
  {
    {
      std::ifstream file(this->filename);
      if (!file.is_open())
      {
        fprintf(stderr, "File not found: %s\n", this->filename.c_str());
        exit(1);
      }
      std::stringstream buffer;
      buffer << file.rdbuf();
      contents = buffer.str();
      file.close();
    } // EOS
    std::string extension = filename.substr(filename.find_last_of(".") + 1);
    res->headers["Content-Type"] = "image/" + extension;
    res->headers["Content-Length"] = std::to_string(contents.size());
    res->body = contents;
    res->version = "HTTP/1.1";
    res->status = "200";
    res->message = "OK";
  }
};

#endif // !#ifndef COW_DOCTYPES_H
