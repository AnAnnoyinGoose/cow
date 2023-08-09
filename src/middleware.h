#ifndef MIDDLEWARE
#define MIDDLEWARE
#include "doctype.h"
#include <chrono>

namespace core {

class MiddleWare {
public:
  std::string name, version;
  virtual void *process(void *) = 0;
  virtual ~MiddleWare() {}
  MiddleWare(const std::string &name, const std::string &version)
      : name(name), version(version) {}
};

MiddleWare *findMiddlewareByName(const std::string &name, std::unordered_map<std::string, MiddleWare *> &middlewareMap);
} // namespace core

enum class Level { lDEBUG, lINFO, lWARN, lERROR, lFATAL };




namespace builtIn {
void useLoggerIfFound(
    std::unordered_map<std::string, core::MiddleWare *> &middlewareMap,
    std::string data);
class Logger : public core::MiddleWare {
public:
  Logger() : core::MiddleWare("Logger", "1.0") {}
  virtual void *process(void *data) override {
    auto formated = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::string time = std::ctime(&formated);
    std::cout << "[" << time << "]";
    auto data_ = static_cast<std::string *>(data);
    std::cout << "[" << time << "] " << *data_ << std::endl;
    return nullptr;
  }
  virtual ~Logger() {}
};

class ImageScaler : public core::MiddleWare {};
} // namespace builtIn
#endif // !#ifndef MIDDLEWARE
