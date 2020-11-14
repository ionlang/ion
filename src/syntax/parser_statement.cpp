#include <ionlang/lexical/classifier.h>
#include <ionlang/misc/util.h>
#include <ionlang/syntax/parser.h>

namespace ionlang {
    AstPtrResult<Statement> Parser::parseStatement(const std::shared_ptr<Block>& parent) {
        this->beginSourceLocationMapping();

        AstPtrResult<Statement> statement;

        // TODO: Symbol table is not being used. Variable decls should be registered?
        ionshared::PtrSymbolTable<Construct> symbolTable = parent->symbolTable;

        TokenKind currentTokenKind = this->tokenStream.get().kind;

        /**
         * A built-in type at this position can only mean a
         * variable declaration, same for an identifier followed
         * by another identifier.
         */
        if (Classifier::isBuiltInType(currentTokenKind)
            || (currentTokenKind == TokenKind::Identifier && this->isNext(TokenKind::Identifier))) {
            statement = util::getResultValue(this->parseVariableDeclStmt(parent));
        }
        // If statement.
        else if (currentTokenKind == TokenKind::KeywordIf) {
            statement = util::getResultValue(this->parseIfStmt(parent));
        }
        // Return statement.
        else if (currentTokenKind == TokenKind::KeywordReturn) {
            statement = util::getResultValue(this->parseReturnStmt(parent));
        }
        // Assignment statement.
        else if (this->isNext(TokenKind::SymbolEqual)) {
            statement = util::getResultValue(this->parseAssignmentStmt(parent));
        }
        // Otherwise, it must be a primary expression.
        else {
            AstPtrResult<Expression<>> expression = this->parseExpression(parent);

            IONLANG_PARSER_ASSERT(util::hasValue(expression))
            statement = ExprWrapperStmt::make(util::getResultValue(expression));
            util::getResultValue(statement)->setParent(parent);
            IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolSemiColon))
        }

        return statement;
    }

    AstPtrResult<IfStmt> Parser::parseIfStmt(const std::shared_ptr<Block>& parent) {
        this->beginSourceLocationMapping();

        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::KeywordIf))
        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolParenthesesL))

        AstPtrResult<Expression<>> condition = this->parseExpression(parent);

        IONLANG_PARSER_ASSERT(util::hasValue(condition))
        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolParenthesesR))

        // The block's parent will be filled below.
        AstPtrResult<Block> consequentBlockResult = this->parseBlock(nullptr);

        IONLANG_PARSER_ASSERT(util::hasValue(consequentBlockResult))

        std::shared_ptr<Block> consequentBlock = util::getResultValue(consequentBlockResult);

        // Parse the alternative block if the else keyword is present.
        AstPtrResult<Block> alternativeBlockResult;

        if (this->is(TokenKind::KeywordElse)) {
            IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::KeywordElse))

            // The block's parent will be filled below.
            alternativeBlockResult = this->parseBlock(nullptr);
        }

        ionshared::OptPtr<Block> alternativeBlock = std::nullopt;

        if (util::hasValue(alternativeBlockResult)) {
            alternativeBlock = util::getResultValue(alternativeBlockResult);
        }

        std::shared_ptr<IfStmt> ifStatement = IfStmt::make(
            util::getResultValue(condition),
            consequentBlock,
            alternativeBlock
        );

        ifStatement->setParent(parent);

        // Finally, fill in the gaps.
        consequentBlock->setParent(ifStatement);

        if (util::hasValue(alternativeBlockResult)) {
            util::getResultValue(alternativeBlockResult)->setParent(ifStatement);
        }

        return ifStatement;
    }

    AstPtrResult<ReturnStmt> Parser::parseReturnStmt(const std::shared_ptr<Block>& parent) {
        this->beginSourceLocationMapping();
        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::KeywordReturn))

        AstPtrResult<Expression<>> valueResult{};

        // Return statement contains a value. Parse it and save it.
        if (!this->is(TokenKind::SymbolSemiColon)) {
            valueResult = this->parseExpression(parent);
            IONLANG_PARSER_ASSERT(util::hasValue(valueResult))
        }

        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolSemiColon))

        ionshared::OptPtr<Expression<>> finalValue = std::nullopt;

        if (util::hasValue(valueResult)) {
            finalValue = util::getResultValue(valueResult);
        }

        std::shared_ptr<ReturnStmt> returnStatement = ReturnStmt::make(
            finalValue
        );

        returnStatement->setParent(parent);

        return returnStatement;
    }

    AstPtrResult<AssignmentStmt> Parser::parseAssignmentStmt(
        const std::shared_ptr<Block>& parent
    ) {
        this->beginSourceLocationMapping();

        std::optional<std::string> id = this->parseName();

        IONLANG_PARSER_ASSERT(id.has_value())
        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolEqual))

        AstPtrResult<Expression<>> value = this->parsePrimaryExpr(parent);

        IONLANG_PARSER_ASSERT(util::hasValue(value))
        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolSemiColon))

        std::shared_ptr<AssignmentStmt> assignmentStatement = AssignmentStmt::make(
            Resolvable<VariableDeclStmt>::make(
                ResolvableKind::VariableLike,
                std::make_shared<Identifier>(*id),
                parent
            ),

            util::getResultValue(value)
        );

        assignmentStatement->setParent(parent);

        return assignmentStatement;
    }

    AstPtrResult<VariableDeclStmt> Parser::parseVariableDeclStmt(
        const std::shared_ptr<Block>& parent
    ) {
        this->beginSourceLocationMapping();

        bool isTypeInferred = false;
        AstPtrResult<Resolvable<Type>> typeResult{};

        if (this->is(TokenKind::KeywordLet)) {
            isTypeInferred = true;
            this->tokenStream.skip();
        }
        else {
            // TODO: Not proper parent.
            typeResult = this->parseType(parent);

            IONLANG_PARSER_ASSERT(util::hasValue(typeResult))
        }

        std::optional<std::string> name = this->parseName();

        IONLANG_PARSER_ASSERT(name.has_value())
        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolEqual))

        AstPtrResult<Expression<>> valueResult = this->parseExpression(parent);

        IONLANG_PARSER_ASSERT(util::hasValue(valueResult))

        std::shared_ptr<VariableDeclStmt> variableDecl = VariableDeclStmt::make(
            isTypeInferred
                // TODO: Type must be cloned.
                ? util::getResultValue(valueResult)->type

                : util::getResultValue(typeResult),

            *name,
            util::getResultValue(valueResult)
        );

        variableDecl->setParent(parent);

        //        /**
        //         * Variable declaration construct owns the type. Assign
        //         * the type's parent.
        //         */
        //        if (!isTypeInferred) {
        //            finalType->setParent(variableDecl);
        //        }

        parent->symbolTable->set(variableDecl->name, variableDecl);
        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolSemiColon))
        this->finishSourceLocationMapping(variableDecl);

        return variableDecl;
    }
}
