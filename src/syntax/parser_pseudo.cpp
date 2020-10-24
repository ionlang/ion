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

    std::optional<Arg> Parser::parseArg(std::shared_ptr<Construct> parent) {
        AstPtrResult<Type> type = this->parseType(std::move(parent));

        // TODO: Function returns std::optional<>.
//        IONIR_PARSER_ASSERT(type.hasValue(), Arg)
        if (!util::hasValue(type)) {
            return std::nullopt;
        }

        std::optional<std::string> id = this->parseName();

        // TODO: Function returns std::optional<>.-
        //        IONIR_PARSER_ASSERT(id.hasValue(), Arg)
        if (!id.has_value()) {
            return std::nullopt;
        }

        return std::make_pair(util::getResultValue(type), *id);
    }
}
