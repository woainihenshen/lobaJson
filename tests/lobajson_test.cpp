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

static void test_parse() {
  test_parse_null();
  test_parse_true();
  test_parse_false();
  test_parse_whitespace();
  test_parse_expect_value();
  test_parse_invalid_value();
  test_parse_root_not_sigular();
}

int main() {
  test_parse();
  printf("%d/%d (%3.2f%%) "
         "passed\n", test_pass, test_count, test_pass * 100.0 / test_count);
  return main_ret;
}