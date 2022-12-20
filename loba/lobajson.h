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

struct LobaValue;
struct LobaMember;

union U {
  struct {
    char *s;
    size_t len;
  } s;
  // 这是个声明 还是实体？
  // 表达不清晰
  // double n; 懂了 哦
  struct {
    LobaValue *e;
    size_t size;
  } a;
  struct {
    LobaMember *m;
    size_t size;
  } o;
  double n;
};

struct LobaValue {
  U u;
  LobaType type;
};

// member
struct LobaMember {
  char *k;
  size_t klen;
  LobaValue v;
};

enum {
  lobaParseOk = 0,

  lobaParseExpectValue,
  lobaParseInvalidValue,
  lobaParseRootNotSingular,

  lobaParseNumberTooBig,

  lobaParseMissQuotationMark,
  lobaParseInvalidStringEscape,
  lobaParseInvalidStringChar,

  lobaParseInvalidUnicodeHex,
  lobaParseInvalidUnicodeSurrogate,

  lobaParseMissCommaOrSquareBracket,

  lobaParseMissKey,
  lobaParseMissCommaOrCurlyBracket,
  lobaParseMissColon
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

  int LobaGetBoolean(const LobaValue *v);
  void LobaSetBoolean(LobaValue *v, LobaType b);

  double LobaGetNumber(const LobaValue *v);
  void LobaSetNumber(LobaValue *v, double n);

  const char *LobaGetString(const LobaValue *v);
  void LobaSetString(LobaValue *v, const char *s, size_t len);
  size_t LobaGetStringLength(const LobaValue *v);
  int LobaParseStringRaw(LobaContext *c, char **str, size_t *len);

  size_t LobaGetArraySize(const LobaValue *v);
  LobaValue *LobaGetArrayElement(const LobaValue *v, size_t index);

  size_t LobaGetObjectSize(const LobaValue *v);
  const char *LobaGetObjectKey(const LobaValue *v, size_t index);
  size_t LobaGetObjectKeyLength(const LobaValue *v, size_t index);
  LobaValue *LobaGetObjectValue(const LobaValue *v, size_t index);

 protected:
  int LobaParseValue(LobaContext *c, LobaValue *v);

  void LobaParseWhitespace(LobaContext *c);

  int LobaParseLiteral(LobaContext *c, LobaValue *v,
                       const char *literal, LobaType type);

  int LobaParseNumber(LobaContext *c, LobaValue *v);

  int LobaParseString(LobaContext *c, LobaValue *v);
  int LobaParseArray(LobaContext *c, LobaValue *v);

  int LobaParseObject(LobaContext *c, LobaValue *v);

 private:
  void *LobaContextPush(LobaContext *c, size_t size);

  void *LobaContextPop(LobaContext *c, size_t size);

 private:
  std::string parser_name_;
  const char *LobaParseHex4(const char *p, unsigned int *p_int);
  void LobaEncodeUtf8(LobaContext *p_context, unsigned int u);
};

inline int LobaJson::LobaParseValue(LobaContext *c, LobaValue *v) {
  switch (*c->json) {
    case 'n':return LobaParseLiteral(c, v, "null", LobaType::lobaNull);
    case 't':return LobaParseLiteral(c, v, "true", LobaType::lobaTrue);
    case 'f':return LobaParseLiteral(c, v, "false", LobaType::lobaFalse);
    case '"': return LobaParseString(c, v);
    case '[': return LobaParseArray(c, v);
    case '{': return LobaParseObject(c, v);
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
      LobaFree(v);
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

#define  PUTC(c, ch) do { *(char*)LobaContextPush(c, sizeof(char)) = (ch); } while(0)
#define STRING_ERROR(ret) do { c->top = head; return ret; } while(0)
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
      case '\\':
        switch (*p++) {
          case '\"':PUTC(c, '\"');
            break;
          case '\\':PUTC(c, '\\');
            break;
          case '/':PUTC(c, '/');
            break;
          case 'b':PUTC(c, '\b');
            break;
          case 'f':PUTC(c, '\f');
            break;
          case 'n':PUTC(c, '\n');
            break;
          case 'r':PUTC(c, '\r');
            break;
          case 't':PUTC(c, '\t');
            break;
          case 'u': {
            unsigned u;
            if (!(p = LobaParseHex4(p, &u)))
              STRING_ERROR(lobaParseInvalidUnicodeHex);
            if (u >= 0xD800 && u <= 0xDBFF) {
              if (*p++ != '\\')
                STRING_ERROR(lobaParseInvalidUnicodeSurrogate);
              if (*p++ != 'u')
                STRING_ERROR(lobaParseInvalidUnicodeSurrogate);
              unsigned u2;
              if (!(p = LobaParseHex4(p, &u2)))
                STRING_ERROR(lobaParseInvalidUnicodeHex);
              if (u2 < 0xDC00 || u2 > 0xDFFF)
                STRING_ERROR(lobaParseInvalidUnicodeSurrogate);
              u = (((u - 0xD800) << 10) | (u2 - 0xDC00)) + 0x10000;
            }
            LobaEncodeUtf8(c, u);
            break;
          }
          default:c->top = head;
            return lobaParseInvalidStringEscape;
        }
        break;
      default:
        if (static_cast<unsigned char>(ch) < 0x20) {
          c->top = head;
          return lobaParseInvalidStringChar;
        }
        PUTC(c, ch);
    }
  }
}

void LobaJson::LobaFree(LobaValue *p_value) {
  assert(p_value != nullptr);
  size_t i;
  switch (p_value->type) {
    case LobaType::lobaString:free(p_value->u.s.s);
      break;
    case LobaType::lobaArray:
      for (i = 0; i < p_value->u.a.size; i++) {
        LobaFree(&p_value->u.a.e[i]);
      }
      free(p_value->u.a.e);
      break;
    case LobaType::lobaObject:
        for (i = 0; i < p_value->u.o.size; i++) {
            free(p_value->u.o.m[i].k);
            LobaFree(&p_value->u.o.m[i].v);
        }
        free(p_value->u.o.m);
        break;
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
  return v->u.s.s;
}
void LobaJson::LobaSetString(LobaValue *v, const char *s, size_t len) {
  assert(v != nullptr && (s != nullptr || len == 0));
  LobaFree(v);
  v->u.s.s = (char *)malloc(len + 1);
  memcpy(v->u.s.s, s, len);
  v->u.s.s[len] = '\0';
  v->u.s.len = len;
  v->type = LobaType::lobaString;
}
size_t LobaJson::LobaGetStringLength(const LobaValue *v) {
  assert(v != nullptr && v->type == LobaType::lobaString);
  return v->u.s.len;
}

int LobaJson::LobaGetBoolean(const LobaValue *v) {
  assert(v != nullptr && (v->type == LobaType::lobaTrue ||
      v->type == LobaType::lobaFalse));
  return v->type == LobaType::lobaTrue;
}

void LobaJson::LobaSetBoolean(LobaValue *v, LobaType b) {
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

// 4位16进制字符转换为一个16进制数 /u0001
const char *LobaJson::LobaParseHex4(const char *p, unsigned int *p_int) {
  *p_int = 0;
  for (int i = 0; i < 4; i++) {
    char ch = *p++;
    *p_int <<= 4;
    if (ch >= '0' && ch <= '9') {
      *p_int |= ch - '0';
    } else if (ch >= 'A' && ch <= 'F') {
      *p_int |= ch - ('A' - 10);
    } else if (ch >= 'a' && ch <= 'f') {
      *p_int |= ch - ('a' - 10);
    } else {
      return nullptr;
    }
  }
  return p;
}
void LobaJson::LobaEncodeUtf8(LobaContext *p_context, unsigned int u) {
  if (u <= 0x7F) {
    PUTC(p_context, u & 0xFF);
  } else if (u <= 0x7FF) {
    PUTC(p_context, 0xC0 | ((u >> 6) & 0xFF));
    PUTC(p_context, 0x80 | (u & 0x3F));
  } else if (u <= 0xFFFF) {
    PUTC(p_context, 0xE0 | ((u >> 12) & 0xFF));
    PUTC(p_context, 0x80 | ((u >> 6) & 0x3F));
    PUTC(p_context, 0x80 | (u & 0x3F));
  } else {
    assert(u <= 0x10FFFF);
    PUTC(p_context, 0xF0 | ((u >> 18) & 0xFF));
    PUTC(p_context, 0x80 | ((u >> 12) & 0x3F));
    PUTC(p_context, 0x80 | ((u >> 6) & 0x3F));
    PUTC(p_context, 0x80 | (u & 0x3F));
  }
}

size_t LobaJson::LobaGetArraySize(const LobaValue *v) {
  assert(v != nullptr && v->type == LobaType::lobaArray);
  return v->u.a.size;
}

LobaValue *LobaJson::LobaGetArrayElement(const LobaValue *v, size_t index) {
  assert(v != nullptr && v->type == LobaType::lobaArray);
  assert(index < v->u.a.size);
  return &v->u.a.e[index];
}
int LobaJson::LobaParseArray(LobaContext *c, LobaValue *v) {
  size_t size = 0;
  int ret;
  EXPECT(c, '[');
  LobaParseWhitespace(c);
  if (*c->json == ']') {
    c->json++;
    v->type = LobaType::lobaArray;
    v->u.a.size = 0;
    v->u.a.e = nullptr;
    return lobaParseOk;
  }
  for (;;) {
    LobaParseWhitespace(c);
    LobaValue e;
    LobaInit(&e);
    if ((ret = LobaParseValue(c, &e)) != lobaParseOk) {
      break;
    }
    memcpy(LobaContextPush(c, sizeof(LobaValue)), &e, sizeof(LobaValue));
    size++;
    LobaParseWhitespace(c);
    if (*c->json == ',') {
      c->json++;
    } else if (*c->json == ']') {
      c->json++;
      v->type = LobaType::lobaArray;
      v->u.a.size = size;
      size *= sizeof(LobaValue);
      memcpy(v->u.a.e = (LobaValue *)malloc(size), LobaContextPop(c, size), size);
      return lobaParseOk;
    } else {
      c->top = 0;
      return lobaParseMissCommaOrSquareBracket;
    }
  }
  for (size_t i = 0; i < size; i++) {
    LobaFree((LobaValue *)LobaContextPop(c, sizeof(LobaValue)));
  }
  return ret;
}

int LobaJson::LobaParseObject(LobaContext *c, LobaValue *v) {
  size_t size;
  LobaMember m;
  int ret;
  EXPECT(c, '{');
  LobaParseWhitespace(c);
  if (*c->json == '}') {
    c->json++;
    v->type = LobaType::lobaObject;
    v->u.o.size = 0;
    v->u.o.m = nullptr;
    return lobaParseOk;
  }
  m.k = nullptr;
  size = 0;
  for (;;) {
    char *str;
    LobaInit(&m.v);
    LobaParseWhitespace(c);
    if (*c->json != '"') {
      ret = lobaParseMissKey;
      break;
    }
    if ((ret = LobaParseStringRaw(c, &str, &m.klen)) != lobaParseOk) {
      break;
    }
    memcpy(m.k = (char*)malloc(m.klen + 1), str, m.klen);
    m.k[m.klen] = '\0';

    LobaParseWhitespace(c);
    if (*c->json != ':') {
      ret = lobaParseMissColon;
      break;
    }
    c->json++;
    LobaParseWhitespace(c);

    if ((ret = LobaParseValue(c, &m.v)) != lobaParseOk) {
      break;
    }
    memcpy(LobaContextPush(c, sizeof(LobaMember)), &m, sizeof(LobaMember));
    size++;
    m.k = nullptr;

    LobaParseWhitespace(c);
    if (*c->json == ',') {
      c->json++;
    } else if (*c->json == '}') {
      c->json++;
      v->type = LobaType::lobaObject;
      v->u.o.size = size;
      size *= sizeof(LobaMember);
      memcpy(v->u.o.m = (LobaMember *)malloc(size), LobaContextPop(c, size), size);
      return lobaParseOk;
    } else {
      ret = lobaParseMissCommaOrCurlyBracket;
      break;
    }
  }
  free(m.k);
  for (size_t i = 0; i < size; i++) {
    LobaMember *m = (LobaMember *)LobaContextPop(c, sizeof(LobaMember));
    free(m->k);
    LobaFree(&m->v);
  }
  v->type = lobaNull;
  return ret;
}

size_t LobaJson::LobaGetObjectSize(const LobaValue *v) {
  assert(v != nullptr && v->type == LobaType::lobaObject);
  return v->u.o.size;
}
const char *LobaJson::LobaGetObjectKey(const LobaValue *v, size_t index) {
  assert(v != nullptr && v->type == LobaType::lobaObject);
  assert(index < v->u.o.size);
  return v->u.o.m[index].k;
}
size_t LobaJson::LobaGetObjectKeyLength(const LobaValue *v, size_t index) {
  assert(v != nullptr && v->type == LobaType::lobaObject);
  assert(index < v->u.o.size);
  return v->u.o.m[index].klen;
}
LobaValue *LobaJson::LobaGetObjectValue(const LobaValue *v, size_t index) {
  assert(v != nullptr && v->type == LobaType::lobaObject);
  assert(index < v->u.o.size);
  return &v->u.o.m[index].v;
}
int LobaJson::LobaParseStringRaw(LobaContext *c, char **str, size_t *len) {
  size_t head = c->top;
  const char *p;
  EXPECT(c, '\"');
  p = c->json;
  for (;;) {
    char ch = *p++;
    switch (ch) {
      case '\"':*len = c->top - head;
        *str = (char*)LobaContextPop(c, *len);
        c->json = p;
        return lobaParseOk;
      case '\0':c->top = head;
        return lobaParseMissQuotationMark;
      case '\\':
        switch (*p++) {
          case '\"':PUTC(c, '\"');
            break;
          case '\\':PUTC(c, '\\');
            break;
          case '/':PUTC(c, '/');
            break;
          case 'b':PUTC(c, '\b');
            break;
          case 'f':PUTC(c, '\f');
            break;
          case 'n':PUTC(c, '\n');
            break;
          case 'r':PUTC(c, '\r');
            break;
          case 't':PUTC(c, '\t');
            break;
          case 'u': {
            unsigned u;
            if (!(p = LobaParseHex4(p, &u)))
              STRING_ERROR(lobaParseInvalidUnicodeHex);
            if (u >= 0xD800 && u <= 0xDBFF) {
              if (*p++ != '\\')
                STRING_ERROR(lobaParseInvalidUnicodeSurrogate);
              if (*p++ != 'u')
                STRING_ERROR(lobaParseInvalidUnicodeSurrogate);
              unsigned u2;
              if (!(p = LobaParseHex4(p, &u2)))
                STRING_ERROR(lobaParseInvalidUnicodeHex);
              if (u2 < 0xDC00 || u2 > 0xDFFF)
                STRING_ERROR(lobaParseInvalidUnicodeSurrogate);
              u = (((u - 0xD800) << 10) | (u2 - 0xDC00)) + 0x10000;
            }
            LobaEncodeUtf8(c, u);
            break;
          }
          default:c->top = head;
            return lobaParseInvalidStringEscape;
        }
        break;
      default:
        if (static_cast<unsigned char>(ch) < 0x20) {
          c->top = head;
          return lobaParseInvalidStringChar;
        }
        PUTC(c, ch);
    }
  }
}

#endif  // LOBAJSON_H_