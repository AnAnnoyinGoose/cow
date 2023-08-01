#include <iostream>
#include <string>

#include "cow.h"



auto cow_handler(const core::Request &request) -> core::Response {
  (void)request;
  core::Response response{};
  response.body = HTML_EXAMPLE_PAGE;
  return response;
}


int main(void) {
  core::Server Server(8080, "127.0.0.1");
  

  Server.GET("/cow", cow_handler); 
    
  Server.run();
  return 0;
}
