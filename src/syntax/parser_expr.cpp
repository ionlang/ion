#include <ionlang/syntax/parser.h>

namespace ionlang {
    ionshared::OptPtr<Construct> Parser::parsePrimaryExpr(const ionshared::Ptr<Block> &parent) {
        if (this->is(TokenKind::SymbolParenthesesL)) {
            return this->parseParenthesesExpr(parent);
        }
        else if (this->is(TokenKind::Identifier)) {
            return this->parseIdExpr(parent);
        }

        // TODO: Support unary and binary operation parsing.

        // Otherwise, it must be a literal value.
        return this->parseLiteralValue();
    }

    ionshared::OptPtr<Construct> Parser::parseParenthesesExpr(const ionshared::Ptr<Block> &parent) {
        IONIR_PARSER_ASSERT(this->skipOver(TokenKind::SymbolParenthesesL))

        ionshared::OptPtr<Construct> expr = this->parsePrimaryExpr(parent);

        IONIR_PARSER_ASSERT(this->skipOver(TokenKind::SymbolParenthesesR))

        return expr;
    }

    ionshared::OptPtr<Construct> Parser::parseIdExpr(const ionshared::Ptr<Block> &parent) {
        if (this->isNext(TokenKind::SymbolParenthesesL)) {
            return this->parseCallExpr(parent);
        }

        // TODO: Is this the correct parent for the ref?
        return this->parseRef(parent);
    }

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
