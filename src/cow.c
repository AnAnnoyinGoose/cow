#include "cow.h"
#include <stdlib.h>
void init_routes(struct Router *router) {
  for (int i = 0; i < 63; i++) {
    router->routes[i].callback = NULL;
    router->routes[i].pos = i;
  };
  router->index = 0;
}

void init_socket(struct Server *server) {
  server->fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server->fd < 0)
    exit(1);
  int opt = 1;
  if (setsockopt(server->fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                 sizeof(opt)))
    exit(1);

  struct sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = inet_addr("0.0.0.0");
  address.sin_port = htons(server->port);
  if (bind(server->fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    exit(1);
  if (listen(server->fd, 3) < 0)
    exit(1);
}

void init_server(struct Server *server, int port) {
  init_routes(server->server_router);
  init_socket(server);
#ifdef DEBUG
  printf("Server Instatiated at http://localhost:%i\n",port);
#endif
}
void res_send(struct Response *res, int fd) {
  char response[MAX_BUFFER_SIZE] = "HTTP/1.1 ";
  strcat(response, res->status);
  strcat(response, "\r\n\r\n");
  strcat(response, res->body);
#ifdef DEBUG
  printf("Sending response! data: %s", response);
#endif
  (void)send(fd, response, sizeof(response), 0);
  close(fd);
}
