#ifndef MIDDLEWARE_H
#define MIDDLEWARE_H

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include "./incl/stb_image.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_STATIC
#include "./incl/stb_image_resize.h"
#include "doctype.h"
#include <bits/types/FILE.h>
#include <chrono>
#include <cstdio>
#include <regex>
#include <vector>
namespace core {

class MiddleWare {
public:
  std::string name, version;
  virtual void *process(void *) = 0;
  virtual ~MiddleWare() {}
  MiddleWare(const std::string &name, const std::string &version)
      : name(name), version(version) {}
};

inline MiddleWare *findMiddlewareByName(
    const std::string &name,
    std::unordered_map<std::string, MiddleWare *> &middlewareMap) {
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
    auto formated =
        std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    // HH::MM
    auto time = std::ctime(&formated);
    auto data_ = static_cast<std::string *>(data);
    std::cout << time << "> " << *data_ << std::endl;
    return nullptr;
  }
  virtual ~Logger() {}
};

inline unsigned char *processImage(FILE *file, int w, int h) {
  long long fileSize = 0;
  fseek(file, 0, SEEK_END);
  fileSize = ftell(file);

  unsigned char *buffer = new unsigned char[fileSize];
  if (buffer == nullptr) {
    return nullptr;
  }

  size_t bytes = fread(buffer, 1, fileSize, file);
  if (bytes != (size_t)fileSize) {
    delete[] buffer;
    return nullptr;
  }

  // Decode via STB
  int w_, h_, channels_;
  unsigned char *data =
      stbi_load_from_memory(buffer, (int)fileSize, &w_, &h_, &channels_, 0);
  delete[] buffer;
  if (data == nullptr) {
    return nullptr;
  }

  // Resize
  if (w > 0 && h > 0 && (w != w_ || h != h_)) {
    unsigned char *resized = new unsigned char[w * h * channels_];
    if (resized == nullptr) {
      stbi_image_free(data);
      return nullptr;
    }
    stbir_resize_uint8(data, w_, h_, 0, resized, w, h, 0, channels_);
    stbi_image_free(data);
    data = resized;
    w_ = w;
    h_ = h;

    return data;
  }

  return nullptr;
}

struct ISCase {
  int w, h;
  std::vector<std::string> filenames;
  std::string exp;
  inline bool match(const std::string &fname) const {
    for (auto &str : filenames)
      if (str == fname)
        return true;
    if (std::regex_match(fname, std::regex(exp)))
      return true;
    return false;
  }
  ISCase(int w, int h, const std::string &regex) : w(w), h(h), exp(regex) {};
};

struct ImageScalerConfig {
  std::vector<ISCase> cases;
  inline void addCase(ISCase &ImageCase) { cases.emplace_back(ImageCase); };
};

class ImageScaler : public core::MiddleWare {
private:
  ImageScalerConfig config;

public:
  ImageScaler(ImageScalerConfig &config)
      : core::MiddleWare("ImageScaler", "1.0"), config(config) {}
  virtual void *process(void *data) override {
    if (data == nullptr)
      return nullptr;
    std::string data_ = *(std::string *)data;
    if (data_ == "")
      return nullptr;
    if (this->config.cases.empty())
      return nullptr;
    for (auto &ImageCase : this->config.cases) {
      if (!ImageCase.match(data_))
	continue;
      else {
	FILE* file = fopen(data_.c_str(), "r");
	if (file == nullptr)
	  return nullptr;
	return processImage(file,ImageCase.w, ImageCase.h);
      }
    }
    return nullptr; 
  }

  ~ImageScaler() {}
};
} // namespace builtIn
  //
#endif
