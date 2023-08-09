#include "doctype.h"
#include <iostream>
#include <string>


#define PORT 8080
#define IP "127.0.0.1"
#include "cow.h"

auto cow_handler(const core::Request &request) -> core::Response
{
  (void)request;
  HTML html("cow.html", {{"user", "Goose"}});
  core::Response res = html.send();

  return res;
}

auto redirect_handler(const core::Request &request) -> core::Response
{
  (void)request;
  core::Response res{};
  res.redirect("/cow/test");
  return res;
}

template<class T> int core::Response::sendFile(const T *filetype) {
  std::string str = filetype->filename;
  this->headers["Content-Description"] = "attachment; filename=" + str;
  this->headers["Accept-Ranges"] = "bytes";
  this->headers["Content-Length"] = std::to_string(filetype->contents.size());
  return 0;
}
auto test_handler(const core::Request &request) -> core::Response
{
  (void)request;
  core::Response res{};
  res.sendFile<IMAGE>(new IMAGE("cow.jpg", &res));
  return res;
}



// logger
builtIn::Logger *logger = new builtIn::Logger();



int main(void)
{
  Server.addMiddleware(logger);

  Server.GET("/cow", cow_handler);
  Server.GET("/redirect", redirect_handler);
  Server.GET("/cow/test", test_handler);
  Server.run();
  return 0;
}
