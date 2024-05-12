// Unit test for Google Test's break-on-failure mode.
//
// A user can ask Google Test to seg-fault when an assertion fails, using
// either the GTEST_BREAK_ON_FAILURE environment variable or the
// --gtest_break_on_failure flag.  This file is used for testing such
// functionality.
//
// This program will be invoked from a Python unit test.  It is
// expected to fail.  Don't run it directly.

#include "gtest/gtest.h"

#ifdef GTEST_OS_WINDOWS
#include <stdlib.h>
#include <windows.h>
#endif

namespace {

// A test that's expected to fail.
TEST(Foo, Bar) { EXPECT_EQ(2, 3); }

#if GTEST_HAS_SEH && !defined(GTEST_OS_WINDOWS_MOBILE)
// On Windows Mobile global exception handlers are not supported.
LONG WINAPI
ExitWithExceptionCode(struct _EXCEPTION_POINTERS* exception_pointers) {
  exit(exception_pointers->ExceptionRecord->ExceptionCode);
}
#endif

}  // namespace

int main(int argc, char** argv) {
#ifdef GTEST_OS_WINDOWS
  // Suppresses display of the Windows error dialog upon encountering
  // a general protection fault (segment violation).
  SetErrorMode(SEM_NOGPFAULTERRORBOX | SEM_FAILCRITICALERRORS);

#if GTEST_HAS_SEH && !defined(GTEST_OS_WINDOWS_MOBILE)

  // The default unhandled exception filter does not always exit
  // with the exception code as exit code - for example it exits with
  // 0 for EXCEPTION_ACCESS_VIOLATION and 1 for EXCEPTION_BREAKPOINT
  // if the application is compiled in debug mode. Thus we use our own
  // filter which always exits with the exception code for unhandled
  // exceptions.
  SetUnhandledExceptionFilter(ExitWithExceptionCode);

#endif
#endif  // GTEST_OS_WINDOWS
  testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
