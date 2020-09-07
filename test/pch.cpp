#include <ionlang/misc/static_init.h>
#include "pch.h"

// Testing environment initialization.
int main(int argc, char **argv) {
    // Initialize Google tests.
    ::testing::InitGoogleTest(&argc, argv);

    // Initialize static entities.
    ionlang::static_init::init();

    // Run tests.
    return RUN_ALL_TESTS();
}
