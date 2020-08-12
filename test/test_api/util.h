#pragma once

#include <string>

namespace ionlang::test::util {
    /**
     * Trim a string from the end (right).
     */
    std::string rightTrim(std::string subject);

    /**
     * Trim a string from the beginning (left).
     */
    std::string leftTrim(std::string subject);

    /**
     * Trim a string from both ends (right then left).
     */
    std::string trim(std::string subject);
}
