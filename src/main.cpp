#include <iostream>
#include <string>

#include "cow.h"

int main(void) {
  core::Server Server(8080, "127.0.0.1");

  Server.run();

  return 0;
}
