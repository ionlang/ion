#include <ionlang/lexical/classifier.h>
#include <ionlang/misc/util.h>
#include <ionlang/syntax/parser.h>

namespace ionlang {
    AstPtrResult<Statement> Parser::parseStatement(const ionshared::Ptr<Block> &parent) {
        AstPtrResult<Statement> statement;

        // TODO: Symbol table is not being used. Variable decls should be registered?
        ionshared::PtrSymbolTable<VariableDeclStatement> symbolTable = parent->getSymbolTable();

        TokenKind currentTokenKind = this->tokenStream.get().getKind();

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
            AstPtrResult<Expression> primaryExpr = this->parsePrimaryExpr(parent);

            IONLANG_PARSER_ASSERT(util::hasValue(primaryExpr), Statement)

            statement = std::make_shared<ExprWrapperStatement>(ExprWrapperStatementOpts{
                parent,
                util::getResultValue(primaryExpr)
            });
        }

        return statement;
    }

    AstPtrResult<IfStatement> Parser::parseIfStatement(const ionshared::Ptr<Block> &parent) {
        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::KeywordIf), IfStatement)
        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolParenthesesL), IfStatement)

        AstPtrResult<Value<>> condition = this->parseLiteralFork();

        IONLANG_PARSER_ASSERT(util::hasValue(condition), IfStatement)
        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolParenthesesR), IfStatement)

        // The block's parent will be filled below.
        AstPtrResult<Block> consequentBlockResult = this->parseBlock(nullptr);

        IONLANG_PARSER_ASSERT(util::hasValue(consequentBlockResult), IfStatement)

        ionshared::Ptr<Block> consequentBlock = util::getResultValue(consequentBlockResult);

        // Parse the alternative block if the else keyword is present.
        AstPtrResult<Block> alternativeBlockResult;

        if (this->is(TokenKind::KeywordElse)) {
            IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::KeywordElse), IfStatement)

            // The block's parent will be filled below.
            alternativeBlockResult = this->parseBlock(nullptr);
        }

        ionshared::OptPtr<Block> alternativeBlock = std::nullopt;

        if (util::hasValue(alternativeBlockResult)) {
            alternativeBlock = util::getResultValue(alternativeBlockResult);
        }

        // Make the if statement construct.
        ionshared::Ptr<IfStatement> ifStatement = std::make_shared<IfStatement>(IfStatementOpts{
            parent,
            util::getResultValue(condition),
            consequentBlock,
            alternativeBlock
        });

        // Finally, fill in the gaps.
        consequentBlock->setParent(ifStatement);

        if (util::hasValue(alternativeBlockResult)) {
            util::getResultValue(alternativeBlockResult)->setParent(ifStatement);
        }

        return ifStatement;
    }

    AstPtrResult<ReturnStatement> Parser::parseReturnStatement(const ionshared::Ptr<Block> &parent) {
        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::KeywordReturn), ReturnStatement)

        AstPtrResult<Expression> valueResult;

        // Return statement contains a value. Parse it and save it.
        if (!this->is(TokenKind::SymbolSemiColon)) {
            valueResult = this->parsePrimaryExpr(parent);

            IONLANG_PARSER_ASSERT(util::hasValue(valueResult), ReturnStatement)
        }

        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolSemiColon), ReturnStatement)

        ionshared::OptPtr<Construct> finalValue = std::nullopt;

        if (util::hasValue(valueResult)) {
            finalValue = util::getResultValue(valueResult);
        }

        return std::make_shared<ReturnStatement>(ReturnStatementOpts{
            parent,
            finalValue
        });
    }

    AstPtrResult<AssignmentStatement> Parser::parseAssignmentStatement(const ionshared::Ptr<Block> &parent) {
        std::optional<std::string> id = this->parseId();

        IONLANG_PARSER_ASSERT(id.has_value(), AssignmentStatement)
        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolEqual), AssignmentStatement)

        AstPtrResult<Expression> value = this->parsePrimaryExpr(parent);

        IONLANG_PARSER_ASSERT(util::hasValue(value), AssignmentStatement)
        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolSemiColon), AssignmentStatement)

        return std::make_shared<AssignmentStatement>(AssignmentStatementOpts{
            parent,

            std::make_shared<Ref<VariableDeclStatement>>(
                *id,
                parent,
                RefKind::Variable
            ),

            util::getResultValue(value)
        });
    }
}
