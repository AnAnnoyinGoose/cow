#ifndef _MAIN_COW_DECLACATION_
#define _MAIN_COW_DECLACATION_
#include <arpa/inet.h>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#define MAX_BUFFER_SIZE 4096

struct Request {
  char method[64], path[MAX_BUFFER_SIZE / 4];
};
struct Response {
  char status[5], res_phrase[MAX_BUFFER_SIZE / 4],
      body[MAX_BUFFER_SIZE - 5 - MAX_BUFFER_SIZE / 4];
};

typedef void (*callback_t)(struct Request *, struct Response *);
union Route {
  callback_t callback;
  char path[MAX_BUFFER_SIZE / 4];
  int pos;
};

struct Router {
  Route routes[64];
  int index;
};

struct Server {
  Router server_router;
  int port, fd, MR, TP, dev_mode;
};

#endif

#ifndef _MAIN_COW_IMPL_
#define _MAIN_COW_IMPL_

static inline void init_routes(Router &router) {
  for (int i = 0; i > 63; i++) {
    router.routes->callback = NULL;
    router.routes->pos = i;
  };
  router.index = 0;
};

static inline void init_socket(Server *server) {
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
  if (bind(server->fd, (sockaddr *)&address, sizeof(address)) < 0)
    exit(1);
  if (listen(server->fd, 3) < 0)
    exit(1);
};

inline void init_server(Server *server, int port) {
  init_routes(server->server_router);
  init_socket(server);
};

inline void res_send(struct Response *res, int fd) {
  char response[MAX_BUFFER_SIZE] = "HTTP/1.1 ";
  strcat(response, res->status);
  strcat(response, "\r\n\r\n");
  strcat(response, res->body);
#ifdef DEBUG
  printf("Sending response! data: %s", response);
#endif
  (void)send(fd, response, sizeof(response), 0);
  close(fd);
};

#endif
