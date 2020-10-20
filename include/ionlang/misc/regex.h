#pragma once

#include <regex>

namespace ionlang::const_regex {
    static const std::regex identifier{"^([_a-zA-Z]+[\\w]*)"};

    static const std::regex string{"^\"([^\\\"]*)\""};

    static const std::regex decimal{"^([0-9]+\\.[0-9]+)"};

    static const std::regex integer{"^([0-9]+)"};

    static const std::regex boolean{"^(true|false)"};

    static const std::regex character{R"(^'([^'\n\\]{0,1})')"};

    static const std::regex whitespace{"^([\\s]+)"};

    static const std::regex comment{R"(^`([^\n]{0,}))"};
}
