#include <ionlang/misc/util.h>
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
        return this->parseLiteral();
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

    ionshared::OptPtr<BinaryOperation> Parser::parseBinaryOperation(const ionshared::Ptr<Block> &parent) {
        while (true) {
            std::optional<Operator> operation =
                util::findOperator(this->tokenStream.get().getKind());

            IONIR_PARSER_ASSURE(operation)

            this->tokenStream.skip();

            ionshared::OptPtr<Construct> rightSide = this->parsePrimaryExpr(parent);

            IONIR_PARSER_ASSURE(rightSide)

            // TODO: UNFINISHED!!!! ---------------------------------------
            // ------------------------------------------------------------
            // ------------------------------------------------------------
            // ------------------------------------------------------------
            // ------------------------------------------------------------
            // ------------------------------------------------------------
            // Continue: https://github.com/ionlang/Ion.Net/blob/master/Ion/Parsing/BinaryOpRightSideParser.cs
            // ------------------------------------------------------------

            // TODO: Type should be rightSide's type (but rightSide is a construct. It must be a value... CallExpr should be a expression too).
            return std::make_shared<BinaryOperation>(BinaryOperationOpts{
                nullptr,
                *operation,
                nullptr,
                *rightSide
            });

            // ------------------------------------------------------------
            // ------------------------------------------------------------
            // ------------------------------------------------------------
            // ------------------------------------------------------------
            // ------------------------------------------------------------
            // ------------------------------------------------------------
            // ------------------------------------------------------------
            // ------------------------------------------------------------
        }
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

        return std::make_shared<CallExpr>(
            // TODO: Is this the correct parent for the Ref<Function>?
            std::make_shared<Ref<Function>>(*calleeId, parent),

            // TODO: Parse call args.
            CallArgs()
        );
    }
}
