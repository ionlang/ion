#include <ionlang/const/grammar.h>
#include <ionlang/lexical/classifier.h>
#include <ionlang/syntax/parser.h>

namespace ionlang {
    AstPtrResult<Expression<>> Parser::parseExpression(const ionshared::Ptr<Block>& parent) {
        AstPtrResult<Expression<>> primaryExpression = this->parsePrimaryExpr(parent);

        IONLANG_PARSER_ASSERT(util::hasValue(primaryExpression))

        TokenKind currentTokenKind = this->tokenStream.get().kind;

        /**
         * Check if the current token's kind is an intrinsic operator.
         * If it is, then the expression being parsed is either a unary
         * or binary expression.
         */
        if (Classifier::isIntrinsicOperator(currentTokenKind)) {
            std::optional<IntrinsicOperatorKind> intrinsicOperatorKind =
                util::findIntrinsicOperatorKind(currentTokenKind);

            // TODO
//            if () {
//
//            }

            return util::getResultValue(this->parseOperationExpr(
                0,
                util::getResultValue(primaryExpression),
                parent
                )
            );
        }

        return primaryExpression;
    }

    AstPtrResult<Expression<>> Parser::parsePrimaryExpr(const ionshared::Ptr<Block>& parent) {
        if (this->is(TokenKind::SymbolParenthesesL)) {
            return this->parseParenthesesExpr(parent);
        }
        else if (this->is(TokenKind::Identifier)) {
            return this->parseIdExpr(parent);
        }

        // TODO: Support unary and binary operation parsing.

        // Otherwise, it must be a literal value.
        AstPtrResult<Expression<>> literal = this->parseLiteral();

        IONLANG_PARSER_ASSERT(util::hasValue(literal))

        return util::getResultValue(literal);
    }

    AstPtrResult<Expression<>> Parser::parseParenthesesExpr(const ionshared::Ptr<Block>& parent) {
        this->beginSourceLocationMapping();

        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolParenthesesL))

        AstPtrResult<Expression<>> expression = this->parsePrimaryExpr(parent);

        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolParenthesesR))

        this->finishSourceLocationMapping(util::getResultValue(expression));

        return expression;
    }

    AstPtrResult<Expression<>> Parser::parseIdExpr(const ionshared::Ptr<Block>& parent) {
        this->beginSourceLocationMapping();

        if (this->isNext(TokenKind::SymbolParenthesesL)) {
            return util::getResultValue(this->parseCallExpr(parent));
        }

        // TODO: Is this the correct parent for the ref?
        PtrResolvable<VariableDeclStatement> variableDeclRef =
            util::getResultValue(this->parseResolvable<VariableDeclStatement>(parent));

        this->finishSourceLocationMapping(variableDeclRef);

        return std::make_shared<VariableRefExpr>(variableDeclRef);
    }

    AstPtrResult<Expression<>> Parser::parseOperationExpr(
        uint32_t expressionPrecedence,
        const ionshared::Ptr<Expression<>>& leftSideExpression,
        const ionshared::Ptr<Block>& parent
    ) {
        while (true) {
            std::optional<IntrinsicOperatorKind> operation =
                util::findIntrinsicOperatorKind(this->tokenStream.get().kind);

            IONLANG_PARSER_ASSERT(operation.has_value())

            // TODO
            uint32_t operatorPrecedence = 0/*util::findOperatorPrecedence(operation);*/;

            // TODO: This shouldn't be determined here. Hence, this function should return BinaryOperation (right-side is non-optional).
            // TODO: Also adjust documentation.
            /**
             * If this is a binop that binds at least as tightly as the current binop,
             * consume it, otherwise we are done.
             */
            if (expressionPrecedence >= operatorPrecedence) {
                return leftSideExpression;
            }

            this->tokenStream.skip();

            // TODO: Debugging exception here.
            AstPtrResult<Expression<>> rightSideExpressionResult =
                this->parsePrimaryExpr(parent);

            IONLANG_PARSER_ASSERT(util::hasValue(rightSideExpressionResult))

            // TODO: Adjust documentation.
            /**
             * If BinOp binds less tightly with RHS than the operator after RHS, let
             * the pending operator take RHS as its LHS.
             */
            std::optional<uint32_t> nextOperatorPrecedence =
                -1; /* TODO: util::findOperatorPrecedence(this->token.kind); */

            if (nextOperatorPrecedence >= operatorPrecedence) {
                rightSideExpressionResult = this->parseOperationExpr(
                    operatorPrecedence + 1,
                    util::getResultValue(rightSideExpressionResult),
                    parent
                );
            }

            // TODO: UNFINISHED!!!! ---------------------------------------
            // ------------------------------------------------------------
            // ------------------------------------------------------------
            // ------------------------------------------------------------
            // ------------------------------------------------------------
            // ------------------------------------------------------------
            // Continue: https://github.com/ionlang/Ion.Net/blob/master/Ion/Parsing/BinaryOpRightSideParser.cs
            // ------------------------------------------------------------

            // TODO: Should verify that both left and right side's type are the same?
            return std::make_shared<OperationExpr>(OperationExprOpts{
                leftSideExpression->type,
                *operation,
                leftSideExpression,
                util::getResultValue(rightSideExpressionResult)
            });
        }
    }

    AstPtrResult<CallExpr> Parser::parseCallExpr(const ionshared::Ptr<Block>& parent) {
        this->beginSourceLocationMapping();

        std::optional<std::string> calleeId = this->parseId();

        IONLANG_PARSER_ASSERT(calleeId.has_value())
        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolParenthesesL))

        CallArgs callArgs{};

        while (!this->is(TokenKind::SymbolParenthesesR)) {
            AstPtrResult<Expression<>> primaryExpr = this->parsePrimaryExpr(parent);

            IONLANG_PARSER_ASSERT(util::hasValue(primaryExpr))

            callArgs.push_back(util::getResultValue(primaryExpr));

            if (this->is(TokenKind::SymbolComma) && this->isNext(TokenKind::SymbolParenthesesR)) {
                // TODO: Use DiagnosticBuilder.
                throw std::runtime_error("No lonely leading comma");
            }
            else if (!this->is(TokenKind::SymbolParenthesesR)) {
                IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolComma))
            }
        }

        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolParenthesesR))

        ionshared::Ptr<CallExpr> callExpr = std::make_shared<CallExpr>(
            Resolvable<>::make(ResolvableKind::Prototype, *calleeId, parent),
            callArgs,
            Resolvable<Type>::make(ResolvableKind::PrototypeReturnType, *calleeId, parent)
        );

        this->finishSourceLocationMapping(callExpr);

        return callExpr;
    }
}
