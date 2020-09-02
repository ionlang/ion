#include <ionlang/syntax/parser.h>

namespace ionlang {
    std::optional<std::string> Parser::parseId() {
        IONIR_PARSER_EXPECT(TokenKind::Identifier)

        std::string id = this->tokenStream.get().getValue();

        this->tokenStream.skip();

        return id;
    }

    std::optional<Arg> Parser::parseArg() {
        std::optional<ionshared::Ptr<Type>> type = this->parseType();

        IONIR_PARSER_ASSURE(type)

        std::optional<std::string> id = this->parseId();

        IONIR_PARSER_ASSURE(id)

        return std::make_pair(*type, *id);
    }
}
