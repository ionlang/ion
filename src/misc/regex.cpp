#include <ionlang/misc/regex.h>

namespace ionlang {
    const std::regex Regex::identifier = std::regex("^([_a-zA-Z]+[\\w]*)");

    const std::regex Regex::string = std::regex("^\"([^\\\"]*)\"");

    const std::regex Regex::decimal = std::regex("^([0-9]+\\.[0-9]+)");

    const std::regex Regex::integer = std::regex("^([0-9]+)");

    const std::regex Regex::character = std::regex("^'([^'\\n\\\\]{0,1})'");

    const std::regex Regex::whitespace = std::regex("^([\\s]+)");
}
