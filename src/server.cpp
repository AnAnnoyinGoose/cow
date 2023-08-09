#include "server.h"
#include "doctype.h"
#include "middleware.h" 
#include <cstdint>
// X functions for the methods
#define METHODS                                                                \
  X(GET)                                                                       \
  X(POST)                                                                      \
  X(PUT)                                                                       \
  X(DELETE)                                                                    \
  X(HEAD)                                                                      \
  X(OPTIONS)                                                                   \
  X(CONNECT)


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
int Response::redirect(const std::string &path) {
  this->status = "302";
  this->headers["Location"] = path;
  this->body = "";
  this->headers["Content-Length"] = std::to_string(this->body.size());
  this->headers["Content-Type"] = "text/plain";
  this->version = "HTTP/1.1";
  return 0;
}
void Response::pnf(std::string contents = "404 Not Found") {
  this->status = "404";
  this->message = "Not Found";
  this->body = contents;
  this->headers["Content-Length"] = std::to_string(this->body.size());
  this->headers["Content-Type"] = "text/plain";
  this->version = "HTTP/1.1";
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
    builtIn::useLoggerIfFound(this->middlewares, "New connection from " + std::to_string(addr.sin_addr.s_addr));
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
      builtIn::useLoggerIfFound(this->middlewares, "Connection closed by client");
      break;
    }
    Request request{};
    Response response{};
    {
      std::string str(buffer);
      std::istringstream ss(str);
      ss >> request.method >> request.path >> request.version;
      std::string headers = str.substr(str.find(DELIMETER) + 1);

      {
        std::string data = "New Request\n";
        data +=
            request.method + " " + request.path + " " + request.version + '\n';
        data += headers;
        builtIn::useLoggerIfFound(this->middlewares, data);
      }

    } // EOS

    try {
      response = this->routes[request.path].handler(request);
    } catch (const std::exception &e) {
      if (request.headers["Accept"] == "text/html") {
        response.pnf(HTML_EXAMPLE_PAGE_4XX);
      } else if (this->serveStaticFile(request.path, &response)) {
      }
    }
    response.headers["Server"] = SERVER_VERSION;
    response.socket = socket;
    {
      std::string data = "Sending response\n";
      data += response.message + response.version + '\n';
      for (auto &pair : response.headers) {
        data += pair.first + " " + pair.second + '\n';
      }
      data += response.body;
      builtIn::useLoggerIfFound(this->middlewares, data);


    }
    response.send();
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
  {
    auto logger = findMiddlewareByName("Logger", this->middlewares);
    if (logger != nullptr) {
      std::string data = "New Route " + method + " " + path;
      logger->process(&data);
    }
  }
  routes.emplace(path, Route{method, path, handler});
}
#define X(NAME)                                                                \
  void Server::NAME(const std::string &path,                                   \
                    std::function<Response(const Request &)> handler) {        \
    addRoute(#NAME, path, handler);                                            \
  }

METHODS
#undef X
bool Server::serveStaticFile(const std::string &path, Response *res) {
  if (path.find(".css") != std::string::npos) {
    res->headers["Content-Type"] = "text/css";
  } else if (path.find(".js") != std::string::npos) {
    res->headers["Content-Type"] = "text/javascript";
  } else if (path.find(".png") != std::string::npos) {
    res->headers["Content-Type"] = "image/png";
  } else if (path.find(".jpg") != std::string::npos) {
    res->headers["Content-Type"] = "image/jpeg";
  } else if (path.find(".svg") != std::string::npos) {
    res->headers["Content-Type"] = "image/svg+xml";
  } else {
    builtIn::useLoggerIfFound(this->middlewares, "Static file not supported and/or not found");
    res->pnf("Not Supported and/or not found");
    return false;
  }
  
  std::ifstream file("public/" + path);
  // if file is not found
  if (!file.is_open()) {
    {
      std::string data = path + " not found";
      builtIn::useLoggerIfFound(this->middlewares, data);
    }
    res->pnf("Not Supported and/or not found");
    return false;
  }
  std::stringstream buffer;
  buffer << file.rdbuf();
  res->body = buffer.str();
  return true;
}
void Server::addMiddleware(core::MiddleWare *mw) {
  this->middlewares[mw->name] = mw;
}

} // namespace core
