// Copyright (c) 2022. Yang Zhu

#ifndef LOBAJSON_H_
#define LOBAJSON_H_

#include <cassert>
#include <string>
template<typename T, typename T1>
void EXPECT(T c, T1 ch) {
  assert(*c->json == (ch));
  c->json++;
}

enum LobaType {
  lobaNull,
  lobaFalse,
  lobaTrue,
  lobaNumber,
  lobaString,
  lobaArray,
  lobaObject,
  lobaTestDefaultType,
};

struct LobaValue {
  LobaType type;
};

enum {
  lobaParseOk = 0,
  lobaParseExpectValue,
  lobaParseInvalidValue,
  lobaParseRootNotSingular,
};

struct LobaContext {
  const char *json;
};

class LobaJson {
 public:
  LobaJson() = default;
  ~LobaJson() = default;
  int LobaParse(LobaValue *v, const char *json);
  LobaType LobaGetType(const LobaValue *v);

 protected:
  void LobaParseWhitespace(LobaContext *c);
  int LobaParseValue(LobaContext *c, LobaValue *v);
  int LobaParseNull(LobaContext *c, LobaValue *v);
  int LobaParseTrue(LobaContext *c, LobaValue *v);
  int LobaParseFasle(LobaContext *c, LobaValue *v);

 private:
  std::string parser_name_;
};

inline int LobaJson::LobaParseNull(LobaContext *c, LobaValue *v) {
  EXPECT(c, 'n');
  if (c->json[0] != 'u' || c->json[1] != 'l' || c->json[2] != 'l') {
    return lobaParseInvalidValue;
  }
  c->json += 3;
  v->type = lobaNull;
  return lobaParseOk;
}

int LobaJson::LobaParseTrue(LobaContext *c, LobaValue *v) {
  EXPECT(c, 't');
  if (c->json[0] != 'r' || c->json[1] != 'u' || c->json[2] != 'e') {
    return lobaParseInvalidValue;
  }
  c->json += 3;
  v->type = lobaTrue;
  return lobaParseOk;
}

int LobaJson::LobaParseFasle(LobaContext *c, LobaValue *v) {
  EXPECT(c, 'f');
  if (c->json[0] != 'a' || c->json[1] != 'l'
  || c->json[2] != 's' || c->json[3] != 'e') {
    return lobaParseInvalidValue;
  }
  c->json += 4;
  v->type = lobaFalse;
  return lobaParseOk;
}



inline int LobaJson::LobaParseValue(LobaContext *c, LobaValue *v) {
  switch (*c->json) {
    case 'n':return LobaParseNull(c, v);
    case 't':return LobaParseTrue(c, v);
    case 'f':return LobaParseFasle(c, v);
    case '\0':return lobaParseExpectValue;
    default:return lobaParseInvalidValue;
  }
}

inline void LobaJson::LobaParseWhitespace(LobaContext *c) {
  const char *p = c->json;
  while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') {
    p++;
  }
  c->json = p;
}

inline int LobaJson::LobaParse(LobaValue *v, const char *json) {
  LobaContext c;
  assert(v != nullptr);
  c.json = json;
  v->type = lobaNull;
  LobaParseWhitespace(&c);
  int ret = LobaParseValue(&c, v);
  if (ret == lobaParseOk) {
    LobaParseWhitespace(&c);
    if (*c.json != '\0') {
      return lobaParseRootNotSingular;
    }
  }
  return ret;
}

inline LobaType LobaJson::LobaGetType(const LobaValue *v) {
  assert(v != nullptr);
  return v->type;
}

#endif  // LOBAJSON_H_