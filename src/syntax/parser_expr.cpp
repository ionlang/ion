#include <ionlang/const/grammar.h>
#include <ionlang/lexical/classifier.h>
#include <ionlang/syntax/parser.h>

namespace ionlang {
    AstPtrResult<Expression<>> Parser::parseExpression(const ionshared::Ptr<Block>& parent) {
        AstPtrResult<Expression<>> primaryExpression = this->parsePrimaryExpr(parent);

        IONLANG_PARSER_ASSERT(util::hasValue(primaryExpression))

        // TODO: Change this so the check of whether it's a bin. operation is done here, and the parseOperationExpr() is forcefully invoked.
        /**
         * NOTE: Will return the parsed primary expression if a binary
         * operator isn't present as the next (current) token.
         */
        return util::getResultValue(this->parseOperationExpr(
            0,
            util::getResultValue(primaryExpression),
            parent
        ));
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

        AstPtrResult<Expression<>> expression = this->parseExpression(parent);

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
        uint32_t minimumOperatorPrecedence,
        ionshared::Ptr<Expression<>> leftSideExpression,
        const ionshared::Ptr<Block>& parent
    ) {
        auto isOperatorAndPrecedenceGraterThan = [](TokenKind tokenKind, uint32_t precedence) -> bool {
            return Classifier::isIntrinsicOperator(tokenKind)
                && *util::findIntrinsicOperatorKindPrecedence(tokenKind) >= precedence;
        };

        TokenKind tokenKindBuffer = this->tokenStream.get().kind;

        while (isOperatorAndPrecedenceGraterThan(tokenKindBuffer, minimumOperatorPrecedence)) {
            std::optional<IntrinsicOperatorKind> operatorKind =
                util::findIntrinsicOperatorKind(tokenKindBuffer);

            IONLANG_PARSER_ASSERT(operatorKind.has_value())

            std::optional<uint32_t> operatorPrecedence =
                util::findIntrinsicOperatorKindPrecedence(tokenKindBuffer);

            IONLANG_PARSER_ASSERT(operatorPrecedence.has_value())

            // Skip the operator token.
            this->tokenStream.skip();

            AstPtrResult<Expression<>> rightSidePrimaryExpressionResult =
                this->parsePrimaryExpr(parent);

            tokenKindBuffer = this->tokenStream.get().kind;

            /**
             * TODO: https://en.wikipedia.org/wiki/Operator-precedence_parser.
             * ... or a right-associative operator ... check is missing here.
             */
            while (isOperatorAndPrecedenceGraterThan(tokenKindBuffer, *operatorPrecedence)) {
                rightSidePrimaryExpressionResult = this->parseOperationExpr(
                    util::findIntrinsicOperatorKindPrecedence(tokenKindBuffer).value_or(0),
                    util::getResultValue(rightSidePrimaryExpressionResult),
                    parent
                );

                tokenKindBuffer = this->tokenStream.get().kind;
            }

            // TODO: Make sure both side's type are the same? Or leave it up to type-checking?
            leftSideExpression = std::make_shared<OperationExpr>(OperationExprOpts{
                leftSideExpression->type,
                *operatorKind,
                leftSideExpression,
                util::getResultValue(rightSidePrimaryExpressionResult)
            });
        }

        return leftSideExpression;

//        while (true) {
//            std::optional<IntrinsicOperatorKind> operation =
//                util::findIntrinsicOperatorKind(this->tokenStream.get().kind);
//
//            IONLANG_PARSER_ASSERT(operation.has_value())
//
//            uint32_t operatorPrecedence =
//                Grammar::intrinsicOperatorPrecedences.at(*operation);
//
//            // Skip the operator token.
//            this->tokenStream.skip();
//
//            // TODO: This shouldn't be determined here. Hence, this function should return BinaryOperation (right-side is non-optional).
//            // TODO: Also adjust documentation.
//            /**
//             * If this is a binop that binds at least as tightly as the current binop,
//             * consume it, otherwise we are done.
//             */
//            if (expressionPrecedence >= operatorPrecedence) {
//                return leftSideExpression;
//            }
//
//            // TODO: Debugging exception here.
//            // TODO: The problem is that it's while(true) so it's parsing right side @ EOF, which makes rightSideExprResult null.
//            AstPtrResult<Expression<>> rightSideExpressionResult =
//                this->parseOperationExpr(expressionPrecedence + 1, nullptr, parent);
//
//            IONLANG_PARSER_ASSERT(util::hasValue(rightSideExpressionResult))
//
//            // TODO: UNFINISHED!!!!
//            // ------------------------------------------------------------
//            // Continue: https://github.com/ionlang/Ion.Net/blob/master/Ion/Parsing/BinaryOpRightSideParser.cs
//            // ------------------------------------------------------------
//
//            // TODO: Should verify that both left and right side's type are the same?
//            return std::make_shared<OperationExpr>(OperationExprOpts{
//                leftSideExpression->type,
//                *operation,
//                leftSideExpression,
//                util::getResultValue(rightSideExpressionResult)
//            });
//        }
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
