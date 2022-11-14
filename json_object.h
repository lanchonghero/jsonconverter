#pragma once

#include "json_value_converter.h"

#include <sstream>
#include <ostream>
#include <string>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <unordered_map>

enum AnyFieldType {
  kBool,
  kString,
  kInt32,
  kInt64,
  kUint32,
  kUint64,
  kFloat,
  kDouble,
  kList,  // haven't implemented
  kObject
};

struct AnyField {
  AnyFieldType af_type;
  void* af_data;
};

template<typename ValueType = rapidjson::Value>
class JsonObjectBase {
public:
  JsonObjectBase() = default;
  virtual ~JsonObjectBase() = default;
  
  bool operator== (const JsonObjectBase<ValueType>& o) {
    if (field_map_.size() != o.field_map_.size()) {
      return false;
    }
    for (auto iter = field_map_.begin(); iter != field_map_.end(); ++iter) {
      auto o_iter = o.field_map_.find(iter->first);
      if (o_iter == o.field_map_.end()) {
        return false;
      }
      if (iter->second.af_type != o_iter->second.af_type) {
        return false;
      }
      switch (iter->second.af_type) {
      case AnyFieldType::kBool:
        if (*reinterpret_cast<bool*>(iter->second.af_data) ==
          *reinterpret_cast<bool*>(o_iter->second.af_data)) {
          continue;
        }
        break;
      case AnyFieldType::kString:
        if (*reinterpret_cast<std::string*>(iter->second.af_data) ==
          *reinterpret_cast<std::string*>(o_iter->second.af_data)) {
          continue;
        }
        break;
      case AnyFieldType::kInt32:
        if (*reinterpret_cast<int32_t*>(iter->second.af_data) ==
          *reinterpret_cast<int32_t*>(o_iter->second.af_data)) {
          continue;
        }
        break;
      case AnyFieldType::kUint32:
        if (*reinterpret_cast<uint32_t*>(iter->second.af_data) ==
          *reinterpret_cast<uint32_t*>(o_iter->second.af_data)) {
          continue;
        }
        break;
      case AnyFieldType::kInt64:
        if (*reinterpret_cast<int64_t*>(iter->second.af_data) ==
          *reinterpret_cast<int64_t*>(o_iter->second.af_data)) {
          continue;
        }
        break;
      case AnyFieldType::kUint64:
        if (*reinterpret_cast<uint64_t*>(iter->second.af_data) ==
          *reinterpret_cast<uint64_t*>(o_iter->second.af_data)) {
          continue;
        }
        break;
      case AnyFieldType::kFloat:
        {
          std::stringstream ss1;
          std::stringstream ss2;
          ss1 << *reinterpret_cast<float*>(iter->second.af_data);
          ss2 << *reinterpret_cast<float*>(o_iter->second.af_data);
          if (ss1.str() == ss2.str()) {
            continue;
          }
        }
        break;
      case AnyFieldType::kDouble:
        {
          std::stringstream ss1;
          std::stringstream ss2;
          ss1 << *reinterpret_cast<double*>(iter->second.af_data);
          ss2 << *reinterpret_cast<double*>(o_iter->second.af_data);
          if (ss1.str() == ss2.str()) {
            continue;
          }
        }
        break;
      case AnyFieldType::kObject:
        if (*reinterpret_cast<JsonObjectBase<ValueType>*>(iter->second.af_data) ==
          *reinterpret_cast<JsonObjectBase<ValueType>*>(o_iter->second.af_data)) {
          continue;
        }
        break;
      default:
        continue;
        break;
      }
      return false;
    }
    return true;
  }

  friend std::ostream & operator<< (std::ostream& os, const JsonObjectBase<ValueType>& obj) {
    os << "{";
    for (size_t i = 0; i < obj.field_name_list_.size(); i++) {
      auto& name = obj.field_name_list_[i];
      auto iter = obj.field_map_.find(name);
      if (iter != obj.field_map_.end()) {
        os << name << ":";
        switch (iter->second.af_type) {
        case AnyFieldType::kBool:
          os << *reinterpret_cast<bool*>(iter->second.af_data);
          break;
        case AnyFieldType::kString:
          os << *reinterpret_cast<std::string*>(iter->second.af_data);
          break;
        case AnyFieldType::kInt32:
          os << *reinterpret_cast<int32_t*>(iter->second.af_data);
          break;
        case AnyFieldType::kUint32:
          os << *reinterpret_cast<uint32_t*>(iter->second.af_data);
          break;
        case AnyFieldType::kInt64:
          os << *reinterpret_cast<int64_t*>(iter->second.af_data);
          break;
        case AnyFieldType::kUint64:
          os << *reinterpret_cast<uint64_t*>(iter->second.af_data);
          break;
        case AnyFieldType::kFloat:
          os << *reinterpret_cast<float*>(iter->second.af_data);
          break;
        case AnyFieldType::kDouble:
          os << *reinterpret_cast<double*>(iter->second.af_data);
          break;
        case AnyFieldType::kObject:
          os << *reinterpret_cast<JsonObjectBase<ValueType>*>(iter->second.af_data);
        default:
          break;
        }
        os << ((i == obj.field_name_list_.size() - 1) ? "" : ",");
      }
    }
    os << "}";
    return os;
  }

  void parse(const std::string& json_content) {
    rapidjson::Document doc;
    doc.Parse(json_content.c_str());
    parse_doc(doc);
  }

  void parse_doc(const rapidjson::Value& doc) {
    for (auto iter = doc.MemberBegin(); iter != doc.MemberEnd(); iter++) {
      const auto& name = iter->name.GetString();
      try {
        if (parse_kv(name, iter->value)) {
          continue;
        }
        std::cout << "parse " << name << " failed!\n";
      } catch (const std::exception& e) {
        std::cout << "catch an exception when parse " << name << " error:" << e.what() << "\n";
      }
    }
  }

  bool parse_kv(const std::string& name, const ValueType& value) {
    auto iter = field_map_.find(name);
    if (iter != field_map_.end()) {
      switch (iter->second.af_type) {
      case AnyFieldType::kBool:
        converter_.convert(value, *reinterpret_cast<bool*>(iter->second.af_data));
        break;
      case AnyFieldType::kString:
        converter_.convert(value, *reinterpret_cast<std::string*>(iter->second.af_data));
        break;
      case AnyFieldType::kInt32:
        converter_.convert(value, *reinterpret_cast<int32_t*>(iter->second.af_data));
        break;
      case AnyFieldType::kInt64:
        converter_.convert(value, *reinterpret_cast<int64_t*>(iter->second.af_data));
        break;
      case AnyFieldType::kUint32:
        converter_.convert(value, *reinterpret_cast<uint32_t*>(iter->second.af_data));
        break;
      case AnyFieldType::kUint64:
        converter_.convert(value, *reinterpret_cast<uint64_t*>(iter->second.af_data));
        break;
      case AnyFieldType::kFloat:
        converter_.convert(value, *reinterpret_cast<float*>(iter->second.af_data));
        break;
      case AnyFieldType::kDouble:
        converter_.convert(value, *reinterpret_cast<double*>(iter->second.af_data));
        break;
      case AnyFieldType::kObject:
        reinterpret_cast<JsonObjectBase<ValueType>*>(iter->second.af_data)->parse_doc(value);
        break;
      default:
        return false;
      }
      return true;
    }
    return false;
  }

  void dump_doc(rapidjson::Value& doc, rapidjson::Document::AllocatorType& allocator) {
    for (size_t i = 0; i < field_name_list_.size(); i++) {
      rapidjson::Value value;
      auto& name = field_name_list_[i];
      auto iter = field_map_.find(name);
      if (iter != field_map_.end()) {
        switch (iter->second.af_type) {
        case AnyFieldType::kBool:
          value.SetBool(*reinterpret_cast<bool*>(iter->second.af_data));
          break;
        case AnyFieldType::kString:
            value.SetString(
              reinterpret_cast<std::string*>(iter->second.af_data)->c_str(),
              reinterpret_cast<std::string*>(iter->second.af_data)->length(),
              allocator);
          break;
        case AnyFieldType::kInt32:
          value.SetInt(*reinterpret_cast<int32_t*>(iter->second.af_data));
          break;
        case AnyFieldType::kUint32:
          value.SetUint(*reinterpret_cast<uint32_t*>(iter->second.af_data));
          break;
        case AnyFieldType::kInt64:
          value.SetInt64(*reinterpret_cast<int64_t*>(iter->second.af_data));
          break;
        case AnyFieldType::kUint64:
          value.SetUint64(*reinterpret_cast<uint64_t*>(iter->second.af_data));
          break;
        case AnyFieldType::kFloat:
          value.SetFloat(*reinterpret_cast<float*>(iter->second.af_data));
          break;
        case AnyFieldType::kDouble:
          value.SetDouble(*reinterpret_cast<double*>(iter->second.af_data));
          break;
        case AnyFieldType::kObject:
          value.SetObject();
          (*reinterpret_cast<JsonObjectBase<ValueType>*>(iter->second.af_data)).dump_doc(value, allocator);
          break;
        default:
          break;
        }
        doc.AddMember(rapidjson::Value(name.c_str(), allocator).Move(), value, allocator);
      }
    }
  }

  std::string dump() {
    rapidjson::Document doc;
    rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
    doc.SetObject();

    dump_doc(doc, allocator);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);

    return buffer.GetString();
  }

protected:
  RapidJsonConverter<ValueType> converter_;
  std::unordered_map<std::string, AnyField> field_map_;
  std::vector<std::string> field_name_list_;

  typedef std::vector<std::string>::iterator FieldNameListIter;
};

typedef JsonObjectBase<> JsonObject;

#define DEF_val(realtype, aftype, name, val)                    \
  realtype name = val;                                          \
  FieldNameListIter name##_iter =                               \
    field_name_list_.insert(field_name_list_.end(), #name);     \
  AnyField name##_field = field_map_[#name] = { aftype, &name }

#define DEF_bool(name, val) \
  DEF_val(bool, AnyFieldType::kBool, name, val)

#define DEF_string(name, val) \
  DEF_val(std::string, AnyFieldType::kString, name, val)

#define DEF_int32(name, val) \
  DEF_val(int32_t, AnyFieldType::kInt32, name, val)

#define DEF_int64(name, val) \
  DEF_val(int64_t, AnyFieldType::kInt64, name, val)

#define DEF_uint32(name, val) \
  DEF_val(uint32_t,  AnyFieldType::kUint32, name, val)

#define DEF_uint64(name, val) \
  DEF_val(uint64_t, AnyFieldType::kUint64, name, val)

#define DEF_float(name, val) \
  DEF_val(float, AnyFieldType::kFloat, name, val)

#define DEF_double(name, value) \
  DEF_val(double, AnyFieldType::kDouble, name, val)

#define DEF_object(name, val) \
  DEF_val(decltype(val), AnyFieldType::kObject, name, val)
