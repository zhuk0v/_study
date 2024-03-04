#include <gtest/gtest.h>

#include "lib.h"

TEST(HelloWorldTests, TestVersion) {
  // Expect that the version number is greater than 0
  EXPECT_GT(version(), 0);
}
