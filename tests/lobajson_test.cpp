// Copyright (c) 2022. Yang Zhu
#include "lobajson.h"
#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <iostream>

static int main_ret = 0;
static int test_count = 0;
static int test_pass = 0;

#define EXPECT_EQ_BASE(equality, expect, actual, format) \
    do { \
        test_count++; \
        if (equality) { \
            test_pass++; \
        } else { \
            fprintf(stderr, "%s:%d: expect: " format " actual: " format "\n", \
            __FILE__, __LINE__, expect, actual); \
            main_ret = 1; \
        } \
    } \
    while (0)

#define EXPECT_EQ_INT(expect, actual) \
        EXPECT_EQ_BASE((expect) == (actual), expect, actual, "%d")

#define EXPECT_EQ_DOUBLE(expect, actual) \
        EXPECT_EQ_BASE((expect) == (actual), expect, actual, "%.17g")

#define EXPECT_EQ_STRING(expect, actual, length) \
        EXPECT_EQ_BASE(sizeof(expect) - 1 == (length) && \
        memcmp(expect, actual, length) == 0, expect, actual, "%s")

#define EXPECT_TRUE(actual) EXPECT_EQ_BASE((actual) != 0, "true", "false", "%s")
#define EXPECT_FALSE(actual) EXPECT_EQ_BASE((actual) == 0, "false", "true", "%s")

#define TEST_ERROR(error, json)\
    do {\
        LobaValue v;\
        v.type = lobaTestDefaultType;\
        LobaJson lobajson;\
        EXPECT_EQ_INT(error, lobajson.LobaParse(&v, json));\
        EXPECT_EQ_INT(lobaNull, lobajson.LobaGetType(&v));\
        lobajson.LobaFree(&v); \
    } \
    while(0)

static void test_parse_expect_value_refactor() {
  TEST_ERROR(lobaParseExpectValue, "");
  TEST_ERROR(lobaParseExpectValue, " ");
}

static void test_parse_null() {
  LobaValue v;
  v.type = lobaTestDefaultType;
  LobaJson lobajson;
  EXPECT_EQ_INT(lobaParseOk, lobajson.LobaParse(&v, "null"));
  EXPECT_EQ_INT(lobaNull, lobajson.LobaGetType(&v));
  lobajson.LobaFree(&v);
}

static void test_parse_root_not_sigular() {
  LobaValue v;
  v.type = lobaTestDefaultType;
  LobaJson lobajson;
  EXPECT_EQ_INT(lobaParseRootNotSingular, lobajson.LobaParse(&v, "null nulll"));
  EXPECT_EQ_INT(lobaNull, lobajson.LobaGetType(&v));
  lobajson.LobaFree(&v);
}

static void test_parse_true() {
  LobaValue v;
  v.type = lobaTestDefaultType;
  LobaJson lobajson;
  EXPECT_EQ_INT(lobaParseOk, lobajson.LobaParse(&v, " true "));
  EXPECT_EQ_INT(lobaTrue, lobajson.LobaGetType(&v));
  lobajson.LobaFree(&v);
}
static void test_parse_false() {
  LobaValue v;
  v.type = lobaTestDefaultType;
  LobaJson lobajson;
  EXPECT_EQ_INT(lobaParseOk, lobajson.LobaParse(&v, " false "));
  EXPECT_EQ_INT(lobaFalse, lobajson.LobaGetType(&v));
  lobajson.LobaFree(&v);
}

static void test_parse_whitespace() {
  LobaJson lobajson;
  LobaValue v;
  v.type = lobaTestDefaultType;
  EXPECT_EQ_INT(lobaParseOk, lobajson.LobaParse(&v, "  null"));
  EXPECT_EQ_INT(lobaNull, lobajson.LobaGetType(&v));

  v.type = lobaTestDefaultType;
  EXPECT_EQ_INT(lobaParseOk, lobajson.LobaParse(&v, "  "
                                                    "null"));
  EXPECT_EQ_INT(lobaNull, lobajson.LobaGetType(&v));
  lobajson.LobaFree(&v);
}

static void test_parse_expect_value() {
  LobaJson lobajson;
  LobaValue v;
  v.type = lobaTestDefaultType;
  EXPECT_EQ_INT(lobaParseExpectValue, lobajson.LobaParse(&v, ""));
  EXPECT_EQ_INT(lobaNull, lobajson.LobaGetType(&v));

  v.type = lobaTestDefaultType;

  EXPECT_EQ_INT(lobaParseExpectValue, lobajson.LobaParse(&v, " "));
  EXPECT_EQ_INT(lobaNull, lobajson.LobaGetType(&v));
  lobajson.LobaFree(&v);
}

static void test_parse_invalid_value() {
  LobaJson lobajson;
  LobaValue v;
  v.type = lobaTestDefaultType;
  EXPECT_EQ_INT(lobaParseInvalidValue, lobajson.LobaParse(&v, "nulx"));
  EXPECT_EQ_INT(lobaNull, lobajson.LobaGetType(&v));
  v.type = lobaTestDefaultType;
  EXPECT_EQ_INT(lobaParseInvalidValue, lobajson.LobaParse(&v, "nux"));
  EXPECT_EQ_INT(lobaNull, lobajson.LobaGetType(&v));

  v.type = lobaTestDefaultType;
  EXPECT_EQ_INT(lobaParseInvalidValue, lobajson.LobaParse(&v, "nx"));
  EXPECT_EQ_INT(lobaNull, lobajson.LobaGetType(&v));

  v.type = lobaTestDefaultType;
  EXPECT_EQ_INT(lobaParseInvalidValue, lobajson.LobaParse(&v, "x"));
  EXPECT_EQ_INT(lobaNull, lobajson.LobaGetType(&v));

  v.type = lobaTestDefaultType;

  EXPECT_EQ_INT(lobaParseInvalidValue, lobajson.LobaParse(&v, "?"));
  EXPECT_EQ_INT(lobaNull, lobajson.LobaGetType(&v));
  lobajson.LobaFree(&v);
}

#define TEST_NUMBER(expect, json)\
    do {\
        LobaValue v;\
        v.type = lobaTestDefaultType;\
        LobaJson lobajson;\
        EXPECT_EQ_INT(lobaParseOk, lobajson.LobaParse(&v, json));\
        EXPECT_EQ_INT(lobaNumber, lobajson.LobaGetType(&v));\
        EXPECT_EQ_DOUBLE(expect, lobajson.LobaGetNumber(&v));\
        lobajson.LobaFree(&v); \
    } \
    while(0)

//基本测试合法json数字，边界值，最大值
static void test_parse_number() {
  TEST_NUMBER(0.0, "0");
  TEST_NUMBER(0.0, "-0");
  TEST_NUMBER(0.0, "-0.0");
  TEST_NUMBER(1.0, "1");
  TEST_NUMBER(-1.0, "-1");
  TEST_NUMBER(1.5, "1.5");
  TEST_NUMBER(-1.5, "-1.5");
  TEST_NUMBER(3.1416, "3.1416");
  TEST_NUMBER(1E10, "1E10");
  TEST_NUMBER(1e10, "1e10");
  TEST_NUMBER(1E+10, "1E+10");
  TEST_NUMBER(1E-10, "1E-10");
  TEST_NUMBER(-1E10, "-1E10");
  TEST_NUMBER(-1e10, "-1e10");
  TEST_NUMBER(-1E+10, "-1E+10");
  TEST_NUMBER(-1E-10, "-1E-10");
  TEST_NUMBER(1.234E+10, "1.234E+10");
  TEST_NUMBER(1.234E-10, "1.234E-10");
  TEST_NUMBER(0.0, "1e-10000"); /* must underflow */


#if 1
  TEST_NUMBER(1.0000000000000002, "1.0000000000000002"); /* the smallest number > 1 */
  TEST_NUMBER(4.9406564584124654e-324, "4.9406564584124654e-324"); /* minimum denormal */
  TEST_NUMBER(-4.9406564584124654e-324, "-4.9406564584124654e-324");
  TEST_NUMBER(2.2250738585072009e-308, "2.2250738585072009e-308");  /* Max subnormal double */
  TEST_NUMBER(-2.2250738585072009e-308, "-2.2250738585072009e-308");
  TEST_NUMBER(2.2250738585072014e-308, "2.2250738585072014e-308");  /* Min normal positive double */
  TEST_NUMBER(-2.2250738585072014e-308, "-2.2250738585072014e-308");
  TEST_NUMBER(1.7976931348623157e+308, "1.7976931348623157e+308");  /* Max double */
  TEST_NUMBER(-1.7976931348623157e+308, "-1.7976931348623157e+308");
#endif

}

//不合法json数字
static void test_parse_invalid_numberValue() {
  /* ... */
  /* invalid number */
#if 1
  TEST_ERROR(lobaParseInvalidValue, "+0");
  TEST_ERROR(lobaParseInvalidValue, "+1");
  TEST_ERROR(lobaParseInvalidValue, ".123"); /* at least one digit before '.' */
  TEST_ERROR(lobaParseInvalidValue, "1.");   /* at least one digit after '.' */
  TEST_ERROR(lobaParseInvalidValue, "INF");
  TEST_ERROR(lobaParseInvalidValue, "inf");
  TEST_ERROR(lobaParseInvalidValue, "NAN");
  TEST_ERROR(lobaParseInvalidValue, "nan");
#endif
}

static void test_parse_root_not_singularNumber() {
#if 1
  TEST_ERROR(lobaParseRootNotSingular, "0123"); /* after zero should be '.' , 'E' , 'e' or nothing */
  TEST_ERROR(lobaParseRootNotSingular, "0x0");
  TEST_ERROR(lobaParseRootNotSingular, "0x123");
#endif
}

static void test_parse_number_too_big() {
#if 1
  TEST_ERROR(lobaParseNumberTooBig, "1e309");
  TEST_ERROR(lobaParseNumberTooBig, "-1e309");
#endif
}

#define TEST_STRING(expect, json)\
    do {\
        LobaValue v;\
        v.type = lobaTestDefaultType;\
        LobaJson lobajson;\
        EXPECT_EQ_INT(lobaParseOk, lobajson.LobaParse(&v, json));\
        EXPECT_EQ_INT(lobaString, lobajson.LobaGetType(&v));\
        EXPECT_EQ_STRING(expect, lobajson.LobaGetString(&v), lobajson.LobaGetStringLength(&v)); \
        lobajson.LobaFree(&v); \
    } \
    while(0)

static void test_parse_string() {
  TEST_STRING("", "\"\"");
  TEST_STRING("Hello", "\"Hello\"");
  TEST_STRING("Hello\nWorld", "\"Hello\\nWorld\"");
  TEST_STRING("\x24", "\"\\u0024\"");
#if 0
#endif
}

static void test_parse_missing_quotation_mark() {
#if 1
  TEST_ERROR(lobaParseMissQuotationMark, "\"");
  TEST_ERROR(lobaParseMissQuotationMark, "\"abc");
#endif
}

static void test_parse_invalid_string_escape() {
#if 1
  TEST_ERROR(lobaParseInvalidStringEscape, "\"\\v\"");
  TEST_ERROR(lobaParseInvalidStringEscape, "\"\\'\"");
  TEST_ERROR(lobaParseInvalidStringEscape, "\"\\0\"");
  TEST_ERROR(lobaParseInvalidStringEscape, "\"\\x12\"");
#endif
}

static void test_parse_invalid_string_char() {
#if 1
  TEST_ERROR(lobaParseInvalidStringChar, "\"\x01\"");
  TEST_ERROR(lobaParseInvalidStringChar, "\"\x1F\"");
#endif
}
static void test_parse_string_invalid_unicode_hex() {
  TEST_ERROR(lobaParseInvalidUnicodeHex, "\"\\u\"");
  TEST_ERROR(lobaParseInvalidUnicodeHex, "\"\\u0\"");
  TEST_ERROR(lobaParseInvalidUnicodeHex, "\"\\u01\"");
  TEST_ERROR(lobaParseInvalidUnicodeHex, "\"\\u012\"");
  TEST_ERROR(lobaParseInvalidUnicodeHex, "\"\\u/000\"");
  TEST_ERROR(lobaParseInvalidUnicodeHex, "\"\\uG000\"");
  TEST_ERROR(lobaParseInvalidUnicodeHex, "\"\\u0/00\"");
  TEST_ERROR(lobaParseInvalidUnicodeHex, "\"\\u0G00\"");
  TEST_ERROR(lobaParseInvalidUnicodeHex, "\"\\u00/0\"");
  TEST_ERROR(lobaParseInvalidUnicodeHex, "\"\\u00G0\"");
  TEST_ERROR(lobaParseInvalidUnicodeHex, "\"\\u000/\"");
  TEST_ERROR(lobaParseInvalidUnicodeHex, "\"\\u000G\"");
  TEST_ERROR(lobaParseInvalidUnicodeHex, "\"\\u 123\"");
}




static void test_parse() {
  test_parse_null();
  test_parse_true();
  test_parse_false();
  test_parse_whitespace();
  test_parse_expect_value();
  test_parse_expect_value_refactor();
  test_parse_invalid_value();
  test_parse_root_not_sigular();
  test_parse_number();
  test_parse_invalid_numberValue();
  test_parse_root_not_singularNumber();
  test_parse_number_too_big();
  test_parse_string();
  test_parse_missing_quotation_mark();
  test_parse_invalid_string_char();
  test_parse_invalid_string_escape();
  test_parse_string_invalid_unicode_hex();
}

// test_get_boolean
static void test_get_boolean() {
  LobaValue v;
  v.type = lobaTestDefaultType;
  LobaJson lobajson;
//  EXPECT_EQ_INT(0, lobajson.LobaGetBoolean(&v));
  lobajson.LobaSetBoolean(&v, LobaType::lobaFalse);
  EXPECT_FALSE(lobajson.LobaGetBoolean(&v));
  lobajson.LobaSetBoolean(&v, LobaType::lobaTrue);
  EXPECT_TRUE(lobajson.LobaGetBoolean(&v));
//  EXPECT_EQ_INT(1, lobajson.LobaGetBoolean(&v));
}

// test_get_number
static void test_get_number() {
  LobaValue v;
  v.type = lobaTestDefaultType;
  LobaInit(&v);
  LobaJson lobajson;
  lobajson.LobaSetString(&v, "a", 1);
  lobajson.LobaSetNumber(&v, 1234.5);
  EXPECT_EQ_DOUBLE(1234.5, lobajson.LobaGetNumber(&v));
}

// test_get_string
static void test_get_string() {
  LobaValue v;
  LobaInit(&v);
  LobaJson lobajson;
  lobajson.LobaSetString(&v, "Hello", 5);
  EXPECT_EQ_STRING("Hello", lobajson.LobaGetString(&v), lobajson.LobaGetStringLength(&v));
  lobajson.LobaFree(&v);
}

static void test_get_null() {
  LobaValue v;
  v.type = lobaTestDefaultType;
  LobaJson lobajson;
  LobaInit(&v);
  EXPECT_EQ_INT(lobaNull, lobajson.LobaGetType(&v));
}

static void test_access() {
  test_get_null();
  test_get_boolean();
  test_get_number();
  test_get_string();
}


// test unicode


int main() {
  test_parse();
  test_access();
  printf("%d/%d (%3.2f%%) "
         "passed\n", test_pass, test_count, test_pass * 100.0 / test_count);
  return main_ret;
}