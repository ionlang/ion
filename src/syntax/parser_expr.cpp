#include <ionlang/const/grammar.h>
#include <ionlang/lexical/classifier.h>
#include <ionlang/syntax/parser.h>

namespace ionlang {
    AstPtrResult<Expression<>> Parser::parseExpression(const std::shared_ptr<Block>& parent) {
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

    AstPtrResult<Expression<>> Parser::parsePrimaryExpr(const std::shared_ptr<Block>& parent) {
        if (this->is(TokenKind::SymbolParenthesesL)) {
            return this->parseParenthesesExpr(parent);
        }
        else if (this->is(TokenKind::Identifier)) {
            return this->parseIdExpr(parent);
        }

        // TODO: Support unary and binary operation parsing.

        // TODO: Not proper parent.
        // Otherwise, it must be a literal value.
        AstPtrResult<Expression<>> literal = this->parseLiteral(parent);

        IONLANG_PARSER_ASSERT(util::hasValue(literal))

        return util::getResultValue(literal);
    }

    AstPtrResult<Expression<>> Parser::parseParenthesesExpr(const std::shared_ptr<Block>& parent) {
        this->beginSourceLocationMapping();

        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolParenthesesL))

        AstPtrResult<Expression<>> expression = this->parseExpression(parent);

        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolParenthesesR))

        this->finishSourceLocationMapping(util::getResultValue(expression));

        return expression;
    }

    AstPtrResult<Expression<>> Parser::parseIdExpr(const std::shared_ptr<Block>& parent) {
        this->beginSourceLocationMapping();

        if (this->isNext(TokenKind::SymbolParenthesesL)) {
            return util::getResultValue(this->parseCallExpr(parent));
        }
        else if (this->isNext(TokenKind::SymbolBraceL)) {
            return util::getResultValue(this->parseStructDefinitionExpr(parent));
        }

        // TODO: Is this the correct parent for the ref?
        PtrResolvable<VariableDeclStmt> variableDeclRef =
            util::getResultValue(this->parseResolvable<VariableDeclStmt>(parent));

        this->finishSourceLocationMapping(variableDeclRef);

        return std::make_shared<VariableRefExpr>(variableDeclRef);
    }

    AstPtrResult<Expression<>> Parser::parseOperationExpr(
        uint32_t minimumOperatorPrecedence,
        std::shared_ptr<Expression<>> leftSideExpression,
        const std::shared_ptr<Block>& parent
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
            leftSideExpression = OperationExpr::make(
                // TODO: Should copy the type, not use it, because it will be linked.
                leftSideExpression->type,

                *operatorKind,
                leftSideExpression,
                util::getResultValue(rightSidePrimaryExpressionResult)
            );
        }

        return leftSideExpression;
    }

    AstPtrResult<CallExpr> Parser::parseCallExpr(const std::shared_ptr<Block>& parent) {
        this->beginSourceLocationMapping();

        std::optional<std::string> calleeId = this->parseName();

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

        std::shared_ptr<CallExpr> callExpr = CallExpr::make(
            Resolvable<>::make(ResolvableKind::Prototype, *calleeId, parent),
            callArgs,
            Resolvable<Type>::make(ResolvableKind::PrototypeReturnType, *calleeId, parent)
        );

        this->finishSourceLocationMapping(callExpr);

        return callExpr;
    }

    AstPtrResult<StructDefinition> Parser::parseStructDefinitionExpr(
        const std::shared_ptr<Block>& parent
    ) {
        this->beginSourceLocationMapping();

        std::optional<std::string> name = this->parseName();

        IONLANG_PARSER_ASSERT(name.has_value())
        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolBraceL))

        std::vector<std::shared_ptr<Expression<>>> values{};

        while (!this->is(TokenKind::SymbolBraceR)) {
            AstPtrResult<Expression<>> value = this->parseExpression(parent);

            IONLANG_PARSER_ASSERT(util::hasValue(value))

            values.push_back(util::getResultValue(value));
        }

        this->tokenStream.skip();

        std::shared_ptr<StructDefinition> structDefinition = StructDefinition::make(
            Resolvable<Struct>::make(
                ResolvableKind::Struct,
                *name,
                parent
            ),

            values
        );

        this->finishSourceLocationMapping(structDefinition);

        return structDefinition;
    }
}
