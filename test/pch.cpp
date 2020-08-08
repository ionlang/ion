#include "pch.h"

// Testing environment initialization.
int main(int argc, char **argv) {
    // Initialize Google tests.
    ::testing::InitGoogleTest(&argc, argv);

    // Run tests.
    return RUN_ALL_TESTS();
}
