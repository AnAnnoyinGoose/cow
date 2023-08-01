#include "server.h"

namespace core {

int Response::send() const {
  std::string response =
      this->version + " " + this->status + " " + this->message + "\r\n";
  for (auto &pair : this->headers) {
    response += pair.first + ": " + pair.second + "\r\n";
  }
  response += "\r\n" + this->body;
  if (::send(this->socket, response.c_str(), response.size(), 0) < 0) {
    fprintf(stderr, "send: %s\n", strerror(errno));
    exit(1);
  }
  return response.size();
}
Server::Server(int port, const std::string &host) : port(port), host(host) {
  socket = ::socket(AF_INET, SOCK_STREAM, 0);
  if (socket < 0) {
    fprintf(stderr, "socket: %s\n", strerror(errno));
    exit(1);
  }
  int opt = 1;
  if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    fprintf(stderr, "setsockopt: %s\n", strerror(errno));
    exit(1);
  }

  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = inet_addr(this->host.c_str());
  if (bind(socket, (sockaddr *)&addr, sizeof(addr)) < 0) {
    fprintf(stderr, "bind: %s\n", strerror(errno));
    exit(1);
  }
  listen(socket, 5);
  fprintf(stdout, "Listening on port %d\n", port);
}
void Server::run() {
  while (true) {
    sockaddr_in addr{};
    int client = ::accept(socket, nullptr, nullptr);
    if (client < 0) {
      fprintf(stderr, "accept: %s\n", strerror(errno));
      continue;
    }
    fprintf(stdout, "Accepted connection from %s\n", inet_ntoa(addr.sin_addr));
    clients.push_back(std::thread(&Server::clientHandler, this, client));
  }
}
void Server::clientHandler(int socket) {
#define BUFFER_SIZE 1024
#define DELIMETER "\r\n"
  char buffer[BUFFER_SIZE];
  memset(buffer, 0, BUFFER_SIZE);
  while (true) {
    ssize_t n = recv(socket, buffer, BUFFER_SIZE, 0);
    if (n < 0) {
      fprintf(stderr, "recv: %s\n", strerror(errno));
      break;
    }
    if (n == 0) {
      fprintf(stderr, "Connection closed\n");
      break;
    }
    Request request{};
    Response response{};
    {
      std::string str(buffer);
      std::istringstream ss(str);
      ss >> request.method >> request.path >> request.version;
      std::string headers = str.substr(str.find(DELIMETER) + 1);
    } // EOS
    if (request.path == "/") {
      response.version = "HTTP/1.0";
      response.status = "200";
      response.message = "OK";
      response.body = HTML_EXAMPLE_PAGE;
      response.headers["Server"] = SERVER_VERSION;
      response.headers["Content-Type"] = "text/html";
      response.headers["Content-Length"] = std::to_string(response.body.size());
      response.socket = socket;
      response.send();
    } else {
      response.version = "HTTP/1.1";
      response.status = "404";
      response.message = "Not Found";
      response.body = HTML_EXAMPLE_PAGE_4XX;
      response.headers["Server"] = SERVER_VERSION;
      response.headers["Content-Type"] = "text/html";
      response.headers["Content-Length"] = std::to_string(response.body.size());
      response.socket = socket;
      response.send();
    }
  }
  close(socket);
#undef BUFFER_SIZE
#undef DELIMETER
}
Server::~Server() {
  for (auto &thread : clients) {
    thread.join();
  }
  close(socket);
}
void Server::addRoute(const std::string &method, const std::string &path,
                      std::function<Response(const Request &)> handler) {
  routes.emplace(method, Route{method, path, &handler});
}
} // namespace core
  //
