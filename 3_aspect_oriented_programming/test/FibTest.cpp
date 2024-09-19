#include "gtest/gtest.h"
#include "../include/fibonacci.h"

TEST(Fibonacci, validate) {
    EXPECT_EQ(1, fibonacci(1));
    EXPECT_EQ(55, fibonacci(10));
    EXPECT_EQ(6765, fibonacci(20));
    EXPECT_EQ(832040, fibonacci(30));
    EXPECT_EQ(102334155, fibonacci(40));
}
