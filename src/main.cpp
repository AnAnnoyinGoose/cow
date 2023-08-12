#include "doctype.h"
#include "middleware.h"
#include <iostream> 
#include <string>

#define PORT 8080
#define IP "127.0.0.1"
#include "cow.h"

auto cow_handler(const core::Request &request) -> core::Response {
  (void)request;
  HTML html("cow.html", {{"user", "Goose"}});
  core::Response res = html.send();

  return res;
}

auto redirect_handler(const core::Request &request) -> core::Response {
  (void)request;
  core::Response res{};
  res.redirect("/cow/test");
  return res;
}

auto test_handler(const core::Request &request) -> core::Response {
  (void)request;
  core::Response res{};
  res.sendFile<IMAGE>(new IMAGE("meme.jpg", &res));
  return res;
}

// logger
builtIn::Logger *logger = new builtIn::Logger();
builtIn::ImageScalerConfig config;

int main(void) {
  builtIn::ISCase  case1(30,40,"[A-Z]");
  case1.filenames.emplace_back("meme.jpg");
  config.addCase(case1);

  builtIn::ImageScaler* scaler = new builtIn::ImageScaler(config);  
  
  
  Server.addMiddleware(logger);
  Server.addMiddleware(scaler);

  Server.GET("/cow", cow_handler);
  Server.GET("/redirect", redirect_handler);
  Server.GET("/cow/test", test_handler);
  Server.run();
  return 0;
}
