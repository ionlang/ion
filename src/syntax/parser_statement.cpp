#include <ionlang/syntax/parser.h>
#include <ionlang/lexical/classifier.h>

namespace ionlang {
    AstPtrResult<Statement> Parser::parseStatement(const ionshared::Ptr<Block> &parent) {
        AstPtrResult<Statement> statement = nullptr;
        ionshared::PtrSymbolTable<Statement> symbolTable = parent->getSymbolTable();
        TokenKind currentTokenKind = this->tokenStream.get().getKind();

        // A built-in type at this position can only mean a variable declaration.
        if (Classifier::isBuiltInType(currentTokenKind)) {
            statement = this->parseVariableDecl(parent);
        }
        // If statement.
        else if (currentTokenKind == TokenKind::KeywordIf) {
            statement = this->parseIfStatement(parent);
        }
        // Return statement.
        else if (currentTokenKind == TokenKind::KeywordReturn) {
            statement = this->parseReturnStatement(parent);
        }
        // TODO: Use Ast(Ptr)Result<>.
        //        else {
        //            throw ionshared::util::quickError(IONLANG_NOTICE_MISC_UNEXPECTED_TOKEN);
        //        }

        return statement;
    }

    AstPtrResult<IfStatement> Parser::parseIfStatement(const ionshared::Ptr<Block> &parent) {
        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::KeywordIf), IfStatement)
        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolParenthesesL), IfStatement)

        AstPtrResult<Value<>> condition = this->parseLiteral();

        IONLANG_PARSER_ASSERT(condition.hasValue(), IfStatement)
        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolParenthesesR), IfStatement)

        // The block's parent will be filled below.
        AstPtrResult<Block> consequentBlockResult = this->parseBlock(nullptr);

        IONLANG_PARSER_ASSERT(consequentBlockResult.hasValue(), IfStatement)

        ionshared::Ptr<Block> consequentBlock = *consequentBlockResult;

        // Parse the alternative block if the else keyword is present.
        AstPtrResult<Block> alternativeBlockResult = nullptr;

        if (this->is(TokenKind::KeywordElse)) {
            IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::KeywordElse), IfStatement)

            // The block's parent will be filled below.
            alternativeBlockResult = this->parseBlock(nullptr);
        }

        // Make the if statement construct.
        ionshared::Ptr<IfStatement> ifStatement = std::make_shared<IfStatement>(IfStatementOpts{
            parent,
            *condition,
            consequentBlock,
            *alternativeBlockResult
        });

        // Finally, fill in the gaps.
        consequentBlock->setParent(ifStatement);

        if (alternativeBlockResult.hasValue()) {
            alternativeBlockResult.getValueOrNullptr()->setParent(ifStatement);
        }

        return ifStatement;
    }

    AstPtrResult<ReturnStatement> Parser::parseReturnStatement(const ionshared::Ptr<Block> &parent) {
        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::KeywordReturn), ReturnStatement)

        AstPtrResult<Value<>> value = std::nullopt;

        // Return statement contains a value. Parse it and save it.
        if (!this->is(TokenKind::SymbolSemiColon)) {
            value = this->parseLiteral();

            IONLANG_PARSER_ASSERT(value.hasValue(), ReturnStatement)
        }

        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolSemiColon), ReturnStatement)

        return std::make_shared<ReturnStatement>(ReturnStatementOpts{
            parent,
            *value
        });
    }
}
