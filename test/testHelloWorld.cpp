#include "testHelloWorld.h"
#include "gtest/gtest.h"

TEST(BasicTest, HelloWorld) {
    const char* helloWorld = "Hello World!";
    EXPECT_STREQ(helloWorld, helloWorld);
}