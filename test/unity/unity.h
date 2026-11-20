/* =========================================================================
    Unity - A Test Framework for C
    ThrowTheSwitch.org
    MIT License
========================================================================= */

#ifndef UNITY_FRAMEWORK_H
#define UNITY_FRAMEWORK_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

typedef struct {
    uint32_t NumberOfTests;
    uint32_t TestFailures;
    uint32_t TestIgnores;
    uint32_t CurrentTestFailed;
    const char *CurrentTestName;
    uint32_t CurrentTestLineNumber;
} UnityStorage_t;

extern UnityStorage_t Unity;

void UnityBegin(const char *filename);
int UnityEnd(void);
void UnityDefaultTestRun(void (*func)(void), const char *name, const int lineNum);
void UnityAssertEqualNumber(int64_t expected, int64_t actual, const char *msg, uint32_t line);
void UnityAssertTrue(bool condition, const char *msg, uint32_t line);
void UnityFail(const char *msg, uint32_t line);

void setUp(void);
void tearDown(void);

#define UNITY_BEGIN()                   UnityBegin(__FILE__)
#define UNITY_END()                     UnityEnd()
#define RUN_TEST(func)                  UnityDefaultTestRun(func, #func, __LINE__)

#define TEST_ASSERT(cond)               UnityAssertTrue((cond), "Condition evaluated to FALSE", __LINE__)
#define TEST_ASSERT_TRUE(cond)          UnityAssertTrue((cond), "Expected TRUE, got FALSE", __LINE__)
#define TEST_ASSERT_FALSE(cond)         UnityAssertTrue(!(cond), "Expected FALSE, got TRUE", __LINE__)
#define TEST_ASSERT_EQUAL(exp, act)     UnityAssertEqualNumber((int64_t)(exp), (int64_t)(act), "Values not equal", __LINE__)
#define TEST_ASSERT_EQUAL_INT(exp, act) UnityAssertEqualNumber((int64_t)(exp), (int64_t)(act), "Integer mismatch", __LINE__)
#define TEST_ASSERT_EQUAL_UINT32(exp, act) UnityAssertEqualNumber((int64_t)(exp), (int64_t)(act), "UINT32 mismatch", __LINE__)
#define TEST_ASSERT_EQUAL_HEX32(exp, act)  UnityAssertEqualNumber((int64_t)(exp), (int64_t)(act), "HEX32 mismatch", __LINE__)
#define TEST_FAIL_MESSAGE(msg)          UnityFail((msg), __LINE__)

#ifdef __cplusplus
}
#endif

#endif /* UNITY_FRAMEWORK_H */
