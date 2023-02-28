#include <gtest/gtest.h>

#include "mainAppTest.cpp"
#include "parseManifestTest.cpp"
#include "calculateNumContainerTest.cpp"
#include "loadTest.cpp"
#include "unloadTest.cpp"
#include "loadUnloadTest.cpp"
#include "bufferTest.cpp"
#include "moveOrderTest.cpp"

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}