#ifndef LOBAJSON_H__
#define LOBAJSON_H__

#include <cassert>
template <typename T, typename T1>
void EXPECT(T c, T1 ch)
{
    assert(*c->json == (ch));
    c->json++;
}

enum LobaType
{
    lobaNULL,
    lobaFALSE,
    lobaTRUE,
    lobaNUMBER,
    lobaSTRING,
    lobaARRAY,
    lobaOBJECT
};

struct LobaValue
{
    LobaType type;
};

enum
{
    lobaParseOk = 0,
    lobaParseExpectValue,
    lobaParseInvalidValue,
    lobaParseRootNotSingular
};

struct LobaContext
{
    const char *json;
};

class LobaJson
{
public:
    LobaJson(/* args */);
    ~LobaJson();
    int LobaParse(LobaValue *v, const char *json);
    LobaType LobaGetType(const LobaValue *v);

protected:
    void LobaParseWhitespace(LobaContext *c);
    int lobaParseValue(LobaContext *c, LobaValue *v);
    int lobaParseNull(LobaContext *c, LobaValue *v);

private:
};

LobaJson::LobaJson(/* args */)
{
}

LobaJson::~LobaJson()
{
}
inline int LobaJson::lobaParseNull(LobaContext *c, LobaValue *v)
{
    EXPECT(c, 'n');
    if (c->json[0] != 'u' || c->json[1] != 'l' || c->json[2] != 'l')
    {
        return lobaParseInvalidValue;
    }
    c->json += 3;
    v->type = lobaNULL;
    return lobaParseOk;
}

inline int LobaJson::lobaParseValue(LobaContext *c, LobaValue *v)
{
    switch (*c->json)
    {
    case 'n':
        return lobaParseNull(c, v);
    case '\0':
        return lobaParseExpectValue;
    default:
        return lobaParseInvalidValue;
    }
}

inline void LobaJson::LobaParseWhitespace(LobaContext *c)
{
    const char *p = c->json;
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
    {
        p++;
    }
    c->json = p;
}

inline int LobaJson::LobaParse(LobaValue *v, const char *json)
{
    LobaContext c;
    assert(v != nullptr);
    c.json = json;
    v->type = lobaNULL;
    LobaParseWhitespace(&c);

    return lobaParseValue(&c, v);
}

inline LobaType LobaJson::LobaGetType(const LobaValue *v)
{
    assert(v != nullptr);
    return v->type;
}
#endif