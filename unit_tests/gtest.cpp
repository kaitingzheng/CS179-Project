#include <gtest/gtest.h>

TEST(SAMPLETEST, MATH){
    EXPECT_EQ(1000,1000);
}

TEST(SAMPLETEST, NOT_EQUAL){
    ASSERT_NE(100,1000);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}