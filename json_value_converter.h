#pragma once

#include <string>
#include <string.h>
#include <stdint.h>
#include <rapidjson/document.h>
#include <vector>

static const std::string kInvalidParamType = "invalid param type!";

template<typename ValueType>
class IConverter {
public:
  virtual ~IConverter() {}

  virtual void convert(const ValueType& src, bool& dst) = 0;
  virtual void convert(const ValueType& src, std::string& dst) = 0;
  virtual void convert(const ValueType& src, int32_t& dst) = 0;
  virtual void convert(const ValueType& src, int64_t& dst) = 0;
  virtual void convert(const ValueType& src, uint32_t& dst) = 0;
  virtual void convert(const ValueType& src, uint64_t& dst) = 0;
  virtual void convert(const ValueType& src, double& dst) = 0;
  virtual void convert(const ValueType& src, float& dst) = 0;

  // template<typename T>
  // virtual void convert(const ValueType& src, std::vector<T>& dst) = 0;
};


template<typename ValueType>
class RapidJsonConverter : public IConverter<ValueType> {
public:
  virtual void convert(const ValueType& src, bool& dst) {
    if (src.IsInt()) {
      dst = (src.GetInt() != 0);
    } else if (src.IsString()) {
      dst = (std::strcmp(src.GetString(), "true") == 0 || std::strcmp(src.GetString(), "0")) ? true : false;
    } else if (src.IsBool()) {
      dst = src.GetBool();
    } else {
      throw std::runtime_error(kInvalidParamType);
    }
  }

  virtual void convert(const ValueType& src, std::string& dst) {
    if (src.IsString()) {
      dst = std::string(src.GetString());
    } else {
      throw std::runtime_error(kInvalidParamType);
    }
  }

  virtual void convert(const ValueType& src, int32_t& dst) {
    if (src.IsInt()) {
      dst = src.GetInt();
    } else if (src.IsString()) {
      dst = static_cast<int32_t>(std::stol(src.GetString()));
    } else {
      throw std::runtime_error(kInvalidParamType);
    }
  }

  virtual void convert(const ValueType& src, int64_t& dst) {
    if (src.IsInt64()) {
      dst = src.GetInt64();
    } else if (src.IsString()) {
      dst = static_cast<int64_t>(std::stoll(src.GetString()));
    } else {
      throw std::runtime_error(kInvalidParamType);
    }
  }

  virtual void convert(const ValueType& src, uint32_t& dst) {
    if (src.IsUint()) {
      dst = src.GetUint();
    } else if (src.IsString()) {
      dst = static_cast<uint32_t>(std::stoul(src.GetString()));
    } else {
      throw std::runtime_error(kInvalidParamType);
    }
  }

  virtual void convert(const ValueType& src, uint64_t& dst) {
    if (src.IsUint64()) {
      dst = src.GetUint64();
    } else if (src.IsString()) {
      dst = static_cast<uint64_t>(std::stoull(src.GetString()));
    } else {
      throw std::runtime_error(kInvalidParamType);
    }
  }

  virtual void convert(const ValueType& src, double& dst) {
    if (src.IsNumber()) {
      dst = src.GetDouble();
    } else if (src.IsString()) {
      dst = std::stof(src.GetString());
    } else {
      throw std::runtime_error(kInvalidParamType);
    }
  }

  virtual void convert(const ValueType& src, float& dst) {
    if (src.IsNumber()) {
      dst = src.GetDouble();
    } else if (src.IsString()) {
      dst = std::stod(src.GetString());
    } else {
      throw std::runtime_error(kInvalidParamType);
    }
  }
};
