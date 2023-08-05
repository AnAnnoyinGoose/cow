#include "server.h"
#include "doctype.h"
// X functions for the methods
#define METHODS \
  X(GET)        \
  X(POST)       \
  X(PUT)        \
  X(DELETE)     \
  X(HEAD)       \
  X(OPTIONS)    \
  X(CONNECT)

namespace core
{

  int Response::send() const
  {
    std::string response =
        this->version + " " + this->status + " " + this->message + "\r\n";
    for (auto &pair : this->headers)
    {
      response += pair.first + ": " + pair.second + "\r\n";
    }
    response += "\r\n" + this->body;
    if (::send(this->socket, response.c_str(), response.size(), 0) < 0)
    {
      fprintf(stderr, "send: %s\n", strerror(errno));
      exit(1);
    }
    return response.size();
  }
  int Response::redirect(const std::string &path)
  {
    this->status = "302";
    this->headers["Location"] = path;
    this->body = "";
    this->headers["Content-Length"] = std::to_string(this->body.size());
    this->headers["Content-Type"] = "text/plain";
    this->version = "HTTP/1.1";
    return 0;
  }
  void Response::pnf(std::string contents = "404 Not Found")
  {
    this->status = "404";
    this->message = "Not Found";
    this->body = contents;
    this->headers["Content-Length"] = std::to_string(this->body.size());
    this->headers["Content-Type"] = "text/plain";
    this->version = "HTTP/1.1";
  }

  Server::Server(int port, const std::string &host) : port(port), host(host)
  {
    socket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (socket < 0)
    {
      fprintf(stderr, "socket: %s\n", strerror(errno));
      exit(1);
    }
    int opt = 1;
    if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
      fprintf(stderr, "setsockopt: %s\n", strerror(errno));
      exit(1);
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(this->host.c_str());
    if (bind(socket, (sockaddr *)&addr, sizeof(addr)) < 0)
    {
      fprintf(stderr, "bind: %s\n", strerror(errno));
      exit(1);
    }
    listen(socket, 5);
    fprintf(stdout, "Listening on port %d\n", port);
  }
  void Server::run()
  {
    while (true)
    {
      sockaddr_in addr{};
      int client = ::accept(socket, nullptr, nullptr);
      if (client < 0)
      {
        fprintf(stderr, "accept: %s\n", strerror(errno));
        continue;
      }
      fprintf(stdout, "Accepted connection from %s\n", inet_ntoa(addr.sin_addr));
      clients.push_back(std::thread(&Server::clientHandler, this, client));
    }
  }
  void Server::clientHandler(int socket)
  {
#define BUFFER_SIZE 1024
#define DELIMETER "\r\n"
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    while (true)
    {
      ssize_t n = recv(socket, buffer, BUFFER_SIZE, 0);
      if (n < 0)
      {
        fprintf(stderr, "recv: %s\n", strerror(errno));
        break;
      }
      if (n == 0)
      {
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

      try
      {
        response = this->routes[request.path].handler(request);
      }
      catch (const std::exception &e)
      {
        response.status = "404";
        response.message = "Not Found";
        response.body = "404 Not Found";
        response.headers["Content-Length"] = std::to_string(response.body.size());
        response.headers["Content-Type"] = "text/plain";
        response.version = "HTTP/1.1";
      }
      response.socket = socket;
      response.send();
    }
    close(socket);
#undef BUFFER_SIZE
#undef DELIMETER
  }
  Server::~Server()
  {
    for (auto &thread : clients)
    {
      thread.join();
    }
    close(socket);
  }
  void Server::addRoute(const std::string &method, const std::string &path,
                        std::function<Response(const Request &)> handler)
  {
    routes.emplace(path, Route{method, path, handler});
  }

#define X(NAME)                                                       \
  void Server::NAME(const std::string &path,                          \
                    std::function<Response(const Request &)> handler) \
  {                                                                   \
    addRoute(#NAME, path, handler);                                   \
  }

  METHODS
#undef X

} // namespace core
