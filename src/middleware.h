#ifndef MIDDLEWARE_H
#define MIDDLEWARE_H
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

inline MiddleWare *findMiddlewareByName(const std::string &name, std::unordered_map<std::string, MiddleWare *> &middlewareMap) {
  auto it = middlewareMap.find(name);
  if (it != middlewareMap.end()) {
    return it->second;
  }
  return nullptr;
}
} // namespace core

enum class Level { lDEBUG, lINFO, lWARN, lERROR, lFATAL };




namespace builtIn {
inline void useLoggerIfFound(
    std::unordered_map<std::string, core::MiddleWare *> &middlewareMap,
    std::string data) {
  auto logger = findMiddlewareByName("Logger", middlewareMap);
  if (logger != nullptr) {
    logger->process(&data);
  } 
}
class Logger : public core::MiddleWare {
public:
  Logger() : core::MiddleWare("Logger", "1.0") {}
  virtual void *process(void *data) override {
    auto formated = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    // HH::MM 
    auto time = std::ctime(&formated);
    auto data_ = static_cast<std::string *>(data);
    std::cout << time << "> " << *data_ << std::endl;
    return nullptr;
  }
  virtual ~Logger() {}
};

class ImageScaler : public core::MiddleWare {};
} // namespace builtIn
  //
#endif
