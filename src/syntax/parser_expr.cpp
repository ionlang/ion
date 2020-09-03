#include <ionlang/misc/util.h>
#include <ionlang/syntax/parser.h>

namespace ionlang {
    AstPtrResult<> Parser::parsePrimaryExpr(const ionshared::Ptr<Block> &parent) {
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

    AstPtrResult<> Parser::parseParenthesesExpr(const ionshared::Ptr<Block> &parent) {
        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolParenthesesL), Construct)

        AstPtrResult<> expr = this->parsePrimaryExpr(parent);

        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolParenthesesR), Construct)

        return expr;
    }

    AstPtrResult<Construct> Parser::parseIdExpr(const ionshared::Ptr<Block> &parent) {
        if (this->isNext(TokenKind::SymbolParenthesesL)) {
            return this->parseCallExpr(parent);
        }

        // TODO: Is this the correct parent for the ref?
        return this->parseRef(parent);
    }

    AstPtrResult<BinaryOperation> Parser::parseBinaryOperation(const ionshared::Ptr<Block> &parent) {
        while (true) {
            std::optional<Operator> operation =
                util::findOperator(this->tokenStream.get().getKind());

            IONLANG_PARSER_ASSERT(operation.has_value(), BinaryOperation)

            this->tokenStream.skip();

            AstPtrResult<> rightSide = this->parsePrimaryExpr(parent);

            IONLANG_PARSER_ASSERT(rightSide.hasValue(), BinaryOperation)

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

    AstPtrResult<CallExpr> Parser::parseCallExpr(const ionshared::Ptr<Block> &parent) {
        std::optional<std::string> calleeId = this->parseId();

        IONLANG_PARSER_ASSERT(calleeId.has_value(), CallExpr)
        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolParenthesesL), CallExpr)

        // Call contains no arguments.
        if (this->is(TokenKind::SymbolParenthesesR)) {
            // TODO
        }
        else {
            // TODO: Implement.
            throw std::runtime_error("Not implemented; Currently not supported call with arguments");
        }

        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolParenthesesR), CallExpr)
        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolSemiColon), CallExpr)

        return std::make_shared<CallExpr>(
            // TODO: Is this the correct parent for the Ref<Function>?
            std::make_shared<Ref<Function>>(*calleeId, parent),

            // TODO: Parse call args.
            CallArgs()
        );
    }
}
