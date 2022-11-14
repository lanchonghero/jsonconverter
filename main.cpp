#include <iostream>
#include <string>

#include "json_object.h"

class Singer : public JsonObject {
public:
  DEF_string(type, "");
  DEF_int32(age, 0);
};

class Address : public JsonObject {
public:
  DEF_string(country, "china");
  DEF_string(city, "beijing");
  DEF_string(street, "");
};

class Friend : public JsonObject {
public:
  DEF_string(relation, "");
};


class Person : public JsonObject {
public:
  DEF_string(name, "");
  DEF_int32(age, 0);
  DEF_object(address, Address());
};

int main(int argc, char** argv) {
  // std::string json_content = "{\"name\":\"lixiang\",\"age\":41,\"address\":{\"country\":\"China\",\"city\":\"Beijing\",\"street\":\"Unknown\"}}";

  Friend f1, f2, f3;
  f1.relation = "my best friend";
  f2.relation = "new friend";
  f3.relation = "third friend";

  Person p2;
  p2.name = "p2";
  p2.age = 3;
  p2.address.country = "china";
  p2.address.city = "shanghai";
  p2.address.street = "putuo";

  Address addr1;
  addr1.country = "china";
  addr1.city = "beijing";
  addr1.street = "wangjing";

  Person p1;
  p1.name = "p1";
  p1.age = 4;
  p1.address = addr1;


  auto json = p1.dump();
  std::cout << json << std::endl;
  std::cout << p1 << std::endl;
  Person pp;
  pp.parse(json);
  std::cout << "p1 == pp is " << ((p1 == pp) ? "true" : "false") << std::endl;
  assert(p1 == pp);

  return 0;
}
