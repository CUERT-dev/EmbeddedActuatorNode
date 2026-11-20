/* =========================================================================
    Unity - A Test Framework for C
    ThrowTheSwitch.org
    MIT License
========================================================================= */

#include "unity.h"
#include <stdio.h>
#include <stdlib.h>

UnityStorage_t Unity;

void UnityBegin(const char *filename)
{
    Unity.NumberOfTests = 0;
    Unity.TestFailures = 0;
    Unity.TestIgnores = 0;
    Unity.CurrentTestFailed = 0;
    Unity.CurrentTestName = NULL;
    Unity.CurrentTestLineNumber = 0;
    printf("\n=======================================================\n");
    printf(" Unity Automated Test Suite: %s\n", filename);
    printf("=======================================================\n");
}

int UnityEnd(void)
{
    printf("\n-------------------------------------------------------\n");
    printf(" Tests Executed: %u | Failures: %u | Ignored: %u\n",
           Unity.NumberOfTests, Unity.TestFailures, Unity.TestIgnores);
    if (Unity.TestFailures == 0) {
        printf(" \033[32m[PASS] All unit tests completed successfully!\033[0m\n");
    } else {
        printf(" \033[31m[FAIL] Unit test suite encountered failures.\033[0m\n");
    }
    printf("=======================================================\n\n");
    return (int)Unity.TestFailures;
}

void UnityDefaultTestRun(void (*func)(void), const char *name, const int lineNum)
{
    Unity.NumberOfTests++;
    Unity.CurrentTestFailed = 0;
    Unity.CurrentTestName = name;
    Unity.CurrentTestLineNumber = (uint32_t)lineNum;

    setUp();
    func();
    tearDown();

    if (!Unity.CurrentTestFailed) {
        printf("  PASS: %s (line %d)\n", name, lineNum);
    }
}

void UnityAssertEqualNumber(int64_t expected, int64_t actual, const char *msg, uint32_t line)
{
    if (expected != actual) {
        Unity.TestFailures++;
        Unity.CurrentTestFailed = 1;
        printf("  \033[31mFAIL\033[0m: %s (line %u): %s (Expected %ld, Got %ld)\n",
               Unity.CurrentTestName ? Unity.CurrentTestName : "UNKNOWN",
               line, msg, (long)expected, (long)actual);
    }
}

void UnityAssertTrue(bool condition, const char *msg, uint32_t line)
{
    if (!condition) {
        Unity.TestFailures++;
        Unity.CurrentTestFailed = 1;
        printf("  \033[31mFAIL\033[0m: %s (line %u): %s\n",
               Unity.CurrentTestName ? Unity.CurrentTestName : "UNKNOWN",
               line, msg);
    }
}

void UnityFail(const char *msg, uint32_t line)
{
    Unity.TestFailures++;
    Unity.CurrentTestFailed = 1;
    printf("  \033[31mFAIL\033[0m: %s (line %u): %s\n",
           Unity.CurrentTestName ? Unity.CurrentTestName : "UNKNOWN",
           line, msg);
}
