/*
 * run_tests.cpp
 */


#include <gtest/gtest.h>


int main(int argc, char *argv[])
{
  // Google Test will remove its own parameters from argv, so run it first!
  ::testing::InitGoogleTest(&argc, argv);

  int result = RUN_ALL_TESTS();

  return result;
}

