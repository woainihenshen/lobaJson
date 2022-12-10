// Copyright (c) 2022. Yang Zhu

#ifndef LOBAJSON_H_
#define LOBAJSON_H_

#include <cassert>
#include <string>
#include <cmath>
#include <cstring>
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

union U {
  struct {
    char *s;
    size_t len;
  };
  double n;
};

struct LobaValue {
  U u;
  LobaType type;
};

enum {
  lobaParseOk = 0,
  lobaParseExpectValue,
  lobaParseInvalidValue,
  lobaParseRootNotSingular,
  lobaParseNumberTooBig,
  lobaParseMissQuotationMark
};

struct LobaContext {
  const char *json;

  char *stack;
  size_t size; //capacity
  size_t top;//size
};

class LobaJson {
 public:
  LobaJson() = default;
  ~LobaJson() = default;
  int LobaParse(LobaValue *v, const char *json);
  LobaType LobaGetType(const LobaValue *v);

  void LobaFree(LobaValue *p_value);

  int LobaGetBoolen(const LobaValue *v);
  void LobaSetBoolen(LobaValue *v, LobaType b);

  double LobaGetNumber(const LobaValue *v);
  void LobaSetNumber(LobaValue *v, double n);

  const char *LobaGetString(const LobaValue *v);
  void LobaSetString(LobaValue *v, const char *s, size_t len);
  size_t LobaGetStringLength(const LobaValue *v);

 protected:
  int LobaParseValue(LobaContext *c, LobaValue *v);

  void LobaParseWhitespace(LobaContext *c);

  int LobaParseLiteral(LobaContext *c, LobaValue *v,
                       const char *literal, LobaType type);

  int LobaParseNumber(LobaContext *c, LobaValue *v);

  int LobaParseString(LobaContext *c, LobaValue *v);

 private:
  void *LobaContextPush(LobaContext *c, size_t size);

  void *LobaContextPop(LobaContext *c, size_t size);

 private:
  std::string parser_name_;
};

inline int LobaJson::LobaParseValue(LobaContext *c, LobaValue *v) {
  switch (*c->json) {
    case 'n':return LobaParseLiteral(c, v, "null", LobaType::lobaNull);
    case 't':return LobaParseLiteral(c, v, "true", LobaType::lobaTrue);
    case 'f':return LobaParseLiteral(c, v, "false", LobaType::lobaFalse);
    case '"': return LobaParseString(c, v);
    case '\0':return lobaParseExpectValue;
    default:return LobaParseNumber(c, v);
  }
}

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
    for (p++; ISDIGIT(*p); p++) {}
  }
  /* 小数 ... */
  if (*p == '.') {
    p++;
    if (!ISDIGIT(*p)) {
      return lobaParseInvalidValue;
    }
    for (p++; ISDIGIT(*p); p++) {}
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
    for (p++; ISDIGIT(*p); p++) {}
  }

  errno = 0;
  v->u.n = strtod(c->json, NULL);
  if (errno == ERANGE && (v->u.n == HUGE_VAL || v->u.n == -HUGE_VAL)) {
    v->type = LobaType::lobaNull;
    return lobaParseNumberTooBig;
  }
  v->type = LobaType::lobaNumber;
  c->json = p;
  return lobaParseOk;
}

#define LobaInit(v) do { (v)->type = LobaType::lobaNull; } while(0)

inline int LobaJson::LobaParse(LobaValue *v, const char *json) {
  LobaContext c;
  assert(v != nullptr);
  c.json = json;
  c.stack = nullptr;
  c.size = c.top = 0;
  LobaInit(v);
  LobaParseWhitespace(&c);
  int ret = LobaParseValue(&c, v);
  if (ret == lobaParseOk) {
    LobaParseWhitespace(&c);
    if (*c.json != '\0') {
      v->type = LobaType::lobaNull;
      return lobaParseRootNotSingular;
    }
  }
  assert(c.top == 0);
  free(c.stack);
  return ret;
}

inline LobaType LobaJson::LobaGetType(const LobaValue *v) {
  assert(v != nullptr);
  return v->type;
}
int LobaJson::LobaParseLiteral(LobaContext *c, LobaValue *v,
                               const char *literal, LobaType type) {
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
int LobaJson::LobaParseString(LobaContext *c, LobaValue *v) {
  size_t head = c->top;
  size_t len = 0;
  const char *p;
  EXPECT(c, '\"');
  p = c->json;
  for (;;) {
    char ch = *p++;
    switch (ch) {
      case '\"':len = c->top - head;
        LobaSetString(v, static_cast<const char *>(LobaContextPop(c, len)), len);
        c->json = p;
        return lobaParseOk;
      case '\0':c->top = head;
        return lobaParseMissQuotationMark;
      default:auto ret = LobaContextPush(c, sizeof ch);
        *(char *)ret = ch;
    }
  }
}
void LobaJson::LobaFree(LobaValue *p_value) {
  assert(p_value != nullptr);
  if (p_value->type == LobaType::lobaString) {
    free(p_value->u.s);
  }
  p_value->type = LobaType::lobaNull;
}

double LobaJson::LobaGetNumber(const LobaValue *v) {
  assert(v != nullptr && v->type == LobaType::lobaNumber);
  return v->u.n;
}

void LobaJson::LobaSetNumber(LobaValue *v, double n) {
  LobaFree(v);
  v->u.n = n;
  v->type = LobaType::lobaNumber;
}
const char *LobaJson::LobaGetString(const LobaValue *v) {
  assert(v != nullptr && v->type == LobaType::lobaString);
  return v->u.s;
}
void LobaJson::LobaSetString(LobaValue *v, const char *s, size_t len) {
  assert(v != nullptr && (s != nullptr || len == 0));
  LobaFree(v);
  v->u.s = (char *)malloc(len + 1);
  memcpy(v->u.s, s, len);
  v->u.s[len] = '\0';
  v->u.len = len;
  v->type = LobaType::lobaString;
}
size_t LobaJson::LobaGetStringLength(const LobaValue *v) {
  assert(v != nullptr && v->type == LobaType::lobaString);
  return v->u.len;
}
int LobaJson::LobaGetBoolen(const LobaValue *v) {
  assert(v != nullptr && (v->type == LobaType::lobaTrue ||
      v->type == LobaType::lobaFalse));
  return v->type == LobaType::lobaTrue;
}

void LobaJson::LobaSetBoolen(LobaValue *v, LobaType b) {
  LobaFree(v);
  v->type = static_cast<LobaType>(b);
}

#define LobaContextStackSize 256
void *LobaJson::LobaContextPush(LobaContext *c, size_t size) {
  void *ret;
  assert(size > 0);
  if (c->top + size >= c->size) {
    if (c->size == 0) {
      c->size = LobaContextStackSize;
    }
    while (c->top + size >= c->size) {
      c->size += c->size >> 1;
    }
    c->stack = (char *)realloc(c->stack, c->size);
  }
  ret = c->stack + c->top;
  c->top += size; //["][h][e][l][l][o]["]
  return ret;
}

void *LobaJson::LobaContextPop(LobaContext *c, size_t size) {
  assert(c->top >= size);
  return c->stack + (c->top -= size);
}

#endif  // LOBAJSON_H_