#include <ionlang/misc/util.h>
#include <ionlang/syntax/parser.h>

namespace ionlang {
    AstPtrResult<Expression> Parser::parsePrimaryExpr(const ionshared::Ptr<Block> &parent) {
        if (this->is(TokenKind::SymbolParenthesesL)) {
            return this->parseParenthesesExpr(parent);
        }
        else if (this->is(TokenKind::Identifier)) {
            return this->parseIdExpr(parent);
        }

        // TODO: Support unary and binary operation parsing.

        // Otherwise, it must be a literal value.
        AstPtrResult<Value<>> literal = this->parseLiteralFork();

        IONLANG_PARSER_ASSERT(util::hasValue(literal))

        return util::castAstPtrResult<Value<>, Expression>(literal, false);
    }

    AstPtrResult<Expression> Parser::parseParenthesesExpr(const ionshared::Ptr<Block> &parent) {
        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolParenthesesL))

        AstPtrResult<Expression> expr = this->parsePrimaryExpr(parent);

        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolParenthesesR))

        return expr;
    }

    AstPtrResult<Expression> Parser::parseIdExpr(const ionshared::Ptr<Block> &parent) {
        if (this->isNext(TokenKind::SymbolParenthesesL)) {
            return util::getResultValue(this->parseCallExpr(parent));
        }

        // TODO: Is this the correct parent for the ref?
        PtrRef<VariableDeclStatement> variableDeclRef =
            util::getResultValue(this->parseRef<VariableDeclStatement>(parent));

        return std::make_shared<VariableRefExpr>(variableDeclRef);
    }

    AstPtrResult<BinaryOperation> Parser::parseBinaryOperation(const ionshared::Ptr<Block> &parent) {
        while (true) {
            std::optional<Operator> operation =
                util::findOperator(this->tokenStream.get().getKind());

            IONLANG_PARSER_ASSERT(operation.has_value())

            this->tokenStream.skip();

            AstPtrResult<Expression> rightSideResult = this->parsePrimaryExpr(parent);

            IONLANG_PARSER_ASSERT(util::hasValue(rightSideResult))

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
                util::getResultValue(rightSideResult)
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

        IONLANG_PARSER_ASSERT(calleeId.has_value())
        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolParenthesesL))

        // Call contains no arguments.
        if (this->is(TokenKind::SymbolParenthesesR)) {
            // TODO
        }
        else {
            // TODO: Implement.
            throw std::runtime_error("Not implemented; Currently not supported call with arguments");
        }

        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolParenthesesR))
        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolSemiColon))

        return std::make_shared<CallExpr>(
            // TODO: Is this the correct parent for the Ref<Function>?
            std::make_shared<Ref<Function>>(*calleeId, parent, RefKind::Function),

            // TODO: Parse call args.
            CallArgs()
        );
    }
}
