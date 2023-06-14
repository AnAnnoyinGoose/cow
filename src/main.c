#include "cow.h"
#include <stdlib.h>

int main(void){
  struct Server *server = malloc(sizeof(struct Server));
  server->server_router = malloc(sizeof(struct Router));
  init_server(server, 3000);
  
  return 0;
}
