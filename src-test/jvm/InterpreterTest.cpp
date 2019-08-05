#include <gtest/gtest.h>

#include <jvm/Global.hpp>

#define TEST_ARITHMETIC_1(op, op_return, type, param, result, getter, expect) \
  TEST(InterpreterTestCase, Instruction_##op) \
  { \
    coldspot::Interpreter interpreter; \
    uint8_t code[2]; \
    code[0] = coldspot::op; \
    code[1] = coldspot::op_return; \
    coldspot::Frame frame(coldspot::FRAMETYPE_JAVA, 0, 0, code); \
    coldspot::Value operands[1]; \
    frame.operands = operands; \
    frame.push((type) param); \
    coldspot::Value value; \
    interpreter.execute(&frame, &value); \
    expect(result, value.getter()); \
  }

#define TEST_ARITHMETIC_2(op, op_return, type, first, second, result, getter, expect) \
  TEST(InterpreterTestCase, Instruction_##op) \
  { \
    coldspot::Interpreter interpreter; \
    uint8_t code[2]; \
    code[0] = coldspot::op; \
    code[1] = coldspot::op_return; \
    coldspot::Frame frame(coldspot::FRAMETYPE_JAVA, 0, 0, code); \
    coldspot::Value operands[2]; \
    frame.operands = operands; \
    frame.push((type) first); \
    frame.push((type) second); \
    coldspot::Value value; \
    interpreter.execute(&frame, &value); \
    expect(result, value.getter()); \
  }

TEST_ARITHMETIC_2(IADD, IRETURN, jint, 10, 5, 15, as_int, EXPECT_EQ)
TEST_ARITHMETIC_2(LADD, LRETURN, jlong, 10l, 5l, 15l, as_long, EXPECT_EQ)
TEST_ARITHMETIC_2(FADD, FRETURN, jfloat, 10.2, 5.2, 15.4, as_float,
    EXPECT_FLOAT_EQ)
TEST_ARITHMETIC_2(DADD, DRETURN, jdouble, 10.2, 5.2, 15.4, as_double,
    EXPECT_DOUBLE_EQ)

TEST_ARITHMETIC_2(ISUB, IRETURN, jint, 10, 3, 7, as_int, EXPECT_EQ)
TEST_ARITHMETIC_2(LSUB, LRETURN, jlong, 10l, 3l, 7l, as_long, EXPECT_EQ)
TEST_ARITHMETIC_2(FSUB, FRETURN, jfloat, 10.2, 3, 7.2, as_float,
    EXPECT_FLOAT_EQ)
TEST_ARITHMETIC_2(DSUB, DRETURN, jdouble, 10.2, 3, 7.2, as_double,
    EXPECT_DOUBLE_EQ)

TEST_ARITHMETIC_2(IMUL, IRETURN, jint, 10, 5, 50, as_int, EXPECT_EQ)
TEST_ARITHMETIC_2(LMUL, LRETURN, jlong, 10l, 5l, 50l, as_long, EXPECT_EQ)
TEST_ARITHMETIC_2(FMUL, FRETURN, jfloat, 10, 5.5, 55, as_float, EXPECT_FLOAT_EQ)
TEST_ARITHMETIC_2(DMUL, DRETURN, jdouble, 10, 5.5, 55, as_double,
    EXPECT_DOUBLE_EQ)

TEST_ARITHMETIC_2(IDIV, IRETURN, jint, 10, 5, 2, as_int, EXPECT_EQ)
TEST_ARITHMETIC_2(LDIV, LRETURN, jlong, 10l, 5l, 2l, as_long, EXPECT_EQ)
TEST_ARITHMETIC_2(FDIV, FRETURN, jfloat, 55, 5.5, 10, as_float, EXPECT_FLOAT_EQ)
TEST_ARITHMETIC_2(DDIV, DRETURN, jdouble, 55, 5.5, 10, as_double,
    EXPECT_DOUBLE_EQ)

TEST_ARITHMETIC_2(IREM, IRETURN, jint, 10, 4, 2, as_int, EXPECT_EQ)
TEST_ARITHMETIC_2(LREM, LRETURN, jlong, 10l, 4l, 2l, as_long, EXPECT_EQ)
TEST_ARITHMETIC_2(FREM, FRETURN, jfloat, 10.0, 4.0, 2.0, as_float,
    EXPECT_FLOAT_EQ)
TEST_ARITHMETIC_2(DREM, DRETURN, jdouble, 10.0, 4.0, 2.0, as_double,
    EXPECT_DOUBLE_EQ)

TEST_ARITHMETIC_1(INEG, IRETURN, jint, 1, -1, as_int, EXPECT_EQ)
TEST_ARITHMETIC_1(LNEG, LRETURN, jlong, 1, -1, as_long, EXPECT_EQ)
TEST_ARITHMETIC_1(FNEG, FRETURN, jfloat, 1.0, -1.0, as_float, EXPECT_FLOAT_EQ)
TEST_ARITHMETIC_1(DNEG, DRETURN, jdouble, 1.0, -1.0, as_double,
    EXPECT_DOUBLE_EQ)

TEST_ARITHMETIC_2(ISHL, IRETURN, jint, 255, 24, -16777216, as_int, EXPECT_EQ)
TEST_ARITHMETIC_2(ISHR, IRETURN, jint, -16777216, 24, -1, as_int, EXPECT_EQ)
TEST_ARITHMETIC_2(IUSHR, IRETURN, jint, -16777216, 24, 255, as_int, EXPECT_EQ)
TEST_ARITHMETIC_2(LSHL, LRETURN, jlong, 4026531840l, 4l, 64424509440l, as_long,
    EXPECT_EQ)
TEST_ARITHMETIC_2(LSHR, LRETURN, jlong, -64424509440l, 4l, -4026531840, as_long,
    EXPECT_EQ)
TEST_ARITHMETIC_2(LUSHR, LRETURN, jlong, -64424509440l, 4l, 1152921500580315136,
    as_long, EXPECT_EQ)