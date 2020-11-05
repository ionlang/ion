#include <ionlang/syntax/parser.h>

namespace ionlang {
    std::optional<std::string> Parser::parseName() {
        if (!this->is(TokenKind::Identifier)) {
            return std::nullopt;
        }

        std::string name = this->tokenStream.get().value;

        this->tokenStream.skip();

        return name;
    }
}
