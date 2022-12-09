// Copyright (c) 2022. Yang Zhu

#ifndef LOBAJSON_H_
#define LOBAJSON_H_

#include <cassert>
#include <string>
#include <cmath>
template<typename T, typename T1>
void EXPECT(T c, T1 ch) {
  assert(*c->json == (ch));
  c->json++;
}

#define ISDIGIT(ch)         ((ch) >= '0' && (ch) <= '9')
#define ISDIGIT1TO9(ch)     ((ch) >= '1' && (ch) <= '9')

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
  double n;
  LobaType type;
};

enum {
  lobaParseOk = 0,
  lobaParseExpectValue,
  lobaParseInvalidValue,
  lobaParseRootNotSingular,
  lobaParseNumberTooBig
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

  double LobaGetNumber(const LobaValue *v) {
    assert(v != nullptr && v->type == LobaType::lobaNumber);
    return v->n;
  }

 protected:
  void LobaParseWhitespace(LobaContext *c);

  int LobaParseValue(LobaContext *c, LobaValue *v);

  int LobaParseNull(LobaContext *c, LobaValue *v);
  int LobaParseTrue(LobaContext *c, LobaValue *v);
  int LobaParseFalse(LobaContext *c, LobaValue *v);
  int LobaParseLiteral(LobaContext *c, LobaValue *v, const char *literal, LobaType type);

  int LobaParseNumber(LobaContext *c, LobaValue *v);

 private:
  std::string parser_name_;
};

inline void LobaJson::LobaParseWhitespace(LobaContext *c) {
  const char *p = c->json;
  while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') {
    p++;
  }
  c->json = p;
}


int LobaJson::LobaParseNumber(LobaContext *c, LobaValue *v) {

  const char *p = c->json;
  /* 负号 ... */
  if (*p == '-') {
    p++;
  }
  /* 整数 ... */
  if (*p == '0') {
    p++;
  } else {
    if (!ISDIGIT1TO9(*p)) { return lobaParseInvalidValue; }
    for (p++; ISDIGIT(*p); p++);
  }
  /* 小数 ... */
  if (*p == '.') {
    p++;
    if (!ISDIGIT(*p)) {
      return lobaParseInvalidValue;
    }
    for (p++; ISDIGIT(*p); p++);
  }
  /* 指数 ... */
  if (*p == 'e' || *p == 'E') {
    p++;
    if (*p == '+' || *p == '-') {
      p++;
    }
    if (!ISDIGIT(*p)) {
      return lobaParseInvalidValue;
    }
    for (p++; ISDIGIT(*p); p++);
  }

  errno = 0;
  v->n = strtod(c->json, NULL);
  if (errno == ERANGE && (v->n == HUGE_VAL || v->n == -HUGE_VAL)) {
    v->type = LobaType::lobaNull;
    return lobaParseNumberTooBig;
  }
  v->type = LobaType::lobaNumber;
  c->json = p;
  return lobaParseOk;

}

inline int LobaJson::LobaParseValue(LobaContext *c, LobaValue *v) {
  switch (*c->json) {
//    case 'n':return LobaParseNull(c, v);
//    case 't':return LobaParseTrue(c, v);
//    case 'f':return LobaParseFalse(c, v);
    case 'n':return LobaParseLiteral(c, v, "null", LobaType::lobaNull);
    case 't':return LobaParseLiteral(c, v, "true", LobaType::lobaTrue);
    case 'f':return LobaParseLiteral(c, v, "false", LobaType::lobaFalse);

    case '\0':return lobaParseExpectValue;
    default:return LobaParseNumber(c, v);
  }
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
      v->type = LobaType::lobaNull;
      return lobaParseRootNotSingular;
    }
  }
  return ret;
}

inline LobaType LobaJson::LobaGetType(const LobaValue *v) {
  assert(v != nullptr);
  return v->type;
}
int LobaJson::LobaParseLiteral(LobaContext *c, LobaValue *v, const char *literal, LobaType type) {
  size_t i;
  EXPECT(c, literal[0]);
  for (i = 0; literal[i + 1]; i++) {
    if (c->json[i] != literal[i + 1])
      return lobaParseInvalidValue;
  }
  c->json += i;
  v->type = type;
  return lobaParseOk;
}

#endif  // LOBAJSON_H_