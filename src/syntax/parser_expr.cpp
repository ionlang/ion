#include <ionlang/syntax/parser.h>

namespace ionlang {
    ionshared::OptPtr<CallExpr> Parser::parseCallExpr(const ionshared::Ptr<Block> &parent) {
        std::optional<std::string> calleeId = this->parseId();

        IONIR_PARSER_ASSURE(calleeId)
        IONIR_PARSER_ASSERT(this->skipOver(TokenKind::SymbolParenthesesL))

        // Call contains no arguments.
        if (this->is(TokenKind::SymbolParenthesesR)) {
            // TODO
        }
        else {
            // TODO: Implement.
            throw std::runtime_error("Not implemented; Currently not supported call with arguments");
        }

        IONIR_PARSER_ASSERT(this->skipOver(TokenKind::SymbolParenthesesR))
        IONIR_PARSER_ASSERT(this->skipOver(TokenKind::SymbolSemiColon))

        return std::make_shared<CallExpr>(CallExprOpts{
            parent,

            // TODO: Is this the correct parent for the Ref<Function>?
            std::make_shared<Ref<Function>>(*calleeId, parent),

            // TODO: Parse call args.
            nullptr
        });
    }
}
