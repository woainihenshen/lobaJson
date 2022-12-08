// Copyright (c) 2022. Yang Zhu
#include "lobajson.h"
#include <stdio.h>
#include <stdlib.h>

#include <string.h>

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

#define TEST_ERROR(error, json)\
    do {\
        LobaValue v;\
        v.type = lobaTestDefaultType;\
        LobaJson lobajson;\
        EXPECT_EQ_INT(error, lobajson.LobaParse(&v, json));\
        EXPECT_EQ_INT(lobaNull, lobajson.LobaGetType(&v));\
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
}

static void test_parse_root_not_sigular() {
  LobaValue v;
  v.type = lobaTestDefaultType;
  LobaJson lobajson;
  EXPECT_EQ_INT(lobaParseRootNotSingular, lobajson.LobaParse(&v, "null nulll"));
  EXPECT_EQ_INT(lobaNull, lobajson.LobaGetType(&v));
}

static void test_parse_true() {
  LobaValue v;
  v.type = lobaTestDefaultType;
  LobaJson lobajson;
  EXPECT_EQ_INT(lobaParseOk, lobajson.LobaParse(&v, " true "));
  EXPECT_EQ_INT(lobaTrue, lobajson.LobaGetType(&v));
}
static void test_parse_false() {
  LobaValue v;
  v.type = lobaTestDefaultType;
  LobaJson lobajson;
  EXPECT_EQ_INT(lobaParseOk, lobajson.LobaParse(&v, " false "));
  EXPECT_EQ_INT(lobaFalse, lobajson.LobaGetType(&v));
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
}

#define TEST_NUMBER(expect, json)\
    do {\
        LobaValue v;\
        v.type = lobaTestDefaultType;\
        LobaJson lobajson;\
        EXPECT_EQ_INT(lobaParseOk, lobajson.LobaParse(&v, json));\
        EXPECT_EQ_INT(lobaNumber, lobajson.LobaGetType(&v));\
        EXPECT_EQ_INT(expect, lobajson.LobaGetNumber(&v));\
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
}

//不合法json数字
static void test_parse_invalid_numberValue() {
  /* ... */
  /* invalid number */
#if 0
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
#if 0
  TEST_ERROR(lobaParseRootNotSingular, "0123"); /* after zero should be '.' , 'E' , 'e' or nothing */
  TEST_ERROR(lobaParseRootNotSingular, "0x0");
  TEST_ERROR(lobaParseRootNotSingular, "0x123");
#endif
}

static void test_parse_number_too_big() {
#if 0
  TEST_ERROR(lobaParseNumberTooBig, "1e309");
    TEST_ERROR(lobaParseNumberTooBig, "-1e309");
#endif
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
}

int main() {
  test_parse();
  printf("%d/%d (%3.2f%%) "
         "passed\n", test_pass, test_count, test_pass * 100.0 / test_count);
  return main_ret;
}