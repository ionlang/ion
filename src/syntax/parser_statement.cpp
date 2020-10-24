#include <ionlang/lexical/classifier.h>
#include <ionlang/misc/util.h>
#include <ionlang/syntax/parser.h>

namespace ionlang {
    AstPtrResult<Statement> Parser::parseStatement(const std::shared_ptr<Block>& parent) {
        this->beginSourceLocationMapping();

        AstPtrResult<Statement> statement;

        // TODO: Symbol table is not being used. Variable decls should be registered?
        ionshared::PtrSymbolTable<VariableDeclStmt> symbolTable = parent->symbolTable;

        TokenKind currentTokenKind = this->tokenStream.get().kind;

        // A built-in type at this position can only mean a variable declaration.
        if (Classifier::isBuiltInType(currentTokenKind)) {
            statement = util::getResultValue(this->parseVariableDecl(parent));
        }
        // If statement.
        else if (currentTokenKind == TokenKind::KeywordIf) {
            statement = util::getResultValue(this->parseIfStatement(parent));
        }
        // Return statement.
        else if (currentTokenKind == TokenKind::KeywordReturn) {
            statement = util::getResultValue(this->parseReturnStatement(parent));
        }
        // Assignment statement.
        else if (currentTokenKind == TokenKind::Identifier && this->isNext(TokenKind::SymbolEqual)) {
            statement = util::getResultValue(this->parseAssignmentStatement(parent));
        }
        // Otherwise, it must be a primary expression.
        else {
            AstPtrResult<Expression<>> expression = this->parseExpression(parent);

            IONLANG_PARSER_ASSERT(util::hasValue(expression))

            statement = Construct::makeChild<ExprWrapperStmt>(
                parent,
                util::getResultValue(expression)
            );

            IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolSemiColon))
        }

        return statement;
    }

    AstPtrResult<IfStmt> Parser::parseIfStatement(const std::shared_ptr<Block>& parent) {
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

        // Make the if statement construct.
        std::shared_ptr<IfStmt> ifStatement = Construct::makeChild<IfStmt>(
            parent,
            util::getResultValue(condition),
            consequentBlock,
            alternativeBlock
        );

        // Finally, fill in the gaps.
        consequentBlock->parent = ifStatement;

        if (util::hasValue(alternativeBlockResult)) {
            util::getResultValue(alternativeBlockResult)->parent = ifStatement;
        }

        return ifStatement;
    }

    AstPtrResult<ReturnStmt> Parser::parseReturnStatement(const std::shared_ptr<Block>& parent) {
        this->beginSourceLocationMapping();

        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::KeywordReturn))

        AstPtrResult<Expression<>> valueResult;

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

        return Construct::makeChild<ReturnStmt>(
            parent,
            finalValue
        );
    }

    AstPtrResult<AssignmentStmt> Parser::parseAssignmentStatement(
        const std::shared_ptr<Block>& parent
    ) {
        this->beginSourceLocationMapping();

        std::optional<std::string> id = this->parseName();

        IONLANG_PARSER_ASSERT(id.has_value())
        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolEqual))

        AstPtrResult<Expression<>> value = this->parsePrimaryExpr(parent);

        IONLANG_PARSER_ASSERT(util::hasValue(value))
        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolSemiColon))

        return Construct::makeChild<AssignmentStmt>(
            parent,

            std::make_shared<Resolvable<VariableDeclStmt>>(
                ResolvableKind::Variable,
                *id,
                parent
            ),

            util::getResultValue(value)
        );
    }
}
