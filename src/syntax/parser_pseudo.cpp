#include <ionlang/syntax/parser.h>

namespace ionlang {
    std::optional<std::string> Parser::parseId() {
        if (!this->is(TokenKind::Identifier)) {
            return std::nullopt;
        }

        std::string id = this->tokenStream.get().value;

        this->tokenStream.skip();

        return id;
    }

    std::optional<Arg> Parser::parseArg() {
        AstPtrResult<Type> type = this->parseType();

        // TODO: Function returns std::optional<>.
//        IONIR_PARSER_ASSERT(type.hasValue(), Arg)
        if (!util::hasValue(type)) {
            return std::nullopt;
        }

        std::optional<std::string> id = this->parseId();

        // TODO: Function returns std::optional<>.-
        //        IONIR_PARSER_ASSERT(id.hasValue(), Arg)
        if (!id.has_value()) {
            return std::nullopt;
        }

        return std::make_pair(util::getResultValue(type), *id);
    }
}
