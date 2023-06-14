#ifndef _MAIN_COW_IMPL_
#define _MAIN_COW_IMPL_
#include <arpa/inet.h>
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
  union Route routes[64];
  int index;
};

struct Server {
  struct Router* server_router;
  int port, fd, MR, TP, dev_mode;
};

void init_routes(struct Router *router);
void init_socket(struct Server *server);
void init_server(struct Server *server, int port);
void res_send(struct Response *res, int fd);
#endif
