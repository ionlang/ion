#pragma once

#include <regex>

namespace ionlang {
    class Regex {
    public:
        const static std::regex identifier;

        const static std::regex string;

        const static std::regex decimal;

        const static std::regex integer;

        const static std::regex character;

        const static std::regex whitespace;
    };
}
