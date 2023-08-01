#include <arpa/inet.h>
#include <cstring>
#include <functional>
#include <iostream>
#include <netinet/in.h>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <unordered_map>
#include <vector>
#include "expamle_html_pages.h"


// Definitions
#define SERVER_VERSION "cow 1.0.0 BETA"

namespace core {

struct Request {
  std::string method, path, version;

  std::unordered_map<std::string, std::string> headers;
};

struct Response {
  std::string status, message, body, version;
  std::unordered_map<std::string, std::string> headers;
  int socket;
  int send() const;
};

struct Route {
  std::string method;
  std::string path;
  std::function<Response(const Request &)> *handler;
};

class Server {
private:
  int socket, port;
  const std::string host;
  std::unordered_map<std::string, Route> routes;
  std::vector<std::thread> clients;

  void addRoute(const std::string &method, const std::string &path,
                std::function<Response(const Request &)> handler);

  bool serveFile(const std::string &path, Response &res);

  void clientHandler(int socket);

  void init();

public:
  Server(int port, const std::string &host);
  ~Server();
  // Methods
  void GET(const std::string &path,
           std::function<Response(const Request &)> handler);
  void POST(const std::string &path,
            std::function<Response(const Request &)> handler);
  void PUT(const std::string &path,
           std::function<Response(const Request &)> handler);
  void DELETE(const std::string &path,
              std::function<Response(const Request &)> handler);
  void HEAD(const std::string &path,
            std::function<Response(const Request &)> handler);
  void OPTIONS(const std::string &path,
               std::function<Response(const Request &)> handler);
  void CONNECT(const std::string &path,
               std::function<Response(const Request &)> handler);
  void TRACE(const std::string &path,
             std::function<Response(const Request &)> handler);
  void PATCH(const std::string &path,
             std::function<Response(const Request &)> handler);
  // Methods END

  void run();
};

} // namespace core