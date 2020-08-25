#include <ionlang/syntax/parser.h>
#include <ionlang/lexical/classifier.h>

namespace ionlang {
    ionshared::OptPtr<Statement> Parser::parseStatement(const ionshared::Ptr<Block> &parent) {
        ionshared::OptPtr<Statement> statement = std::nullopt;
        ionshared::PtrSymbolTable<Statement> symbolTable = parent->getSymbolTable();
        TokenKind currentTokenKind = this->stream.get().getKind();

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
        //            throw ionshared::Util::quickError(IONLANG_NOTICE_MISC_UNEXPECTED_TOKEN);
        //        }

        return statement;
    }

    ionshared::OptPtr<IfStatement> Parser::parseIfStatement(const ionshared::Ptr<Block> &parent) {
        IONIR_PARSER_ASSERT(this->skipOver(TokenKind::KeywordIf))
        IONIR_PARSER_ASSERT(this->skipOver(TokenKind::SymbolParenthesesL))

        ionshared::OptPtr<Value<>> condition = this->parseValue();

        IONIR_PARSER_ASSURE(condition)
        IONIR_PARSER_ASSERT(this->skipOver(TokenKind::SymbolParenthesesR))

        // The block's parent will be filled below.
        ionshared::OptPtr<Block> consequentBlockResult = this->parseBlock(nullptr);

        IONIR_PARSER_ASSURE(consequentBlockResult)

        ionshared::Ptr<Block> consequentBlock = *consequentBlockResult;

        // Parse the alternative block if the else keyword is present.
        ionshared::OptPtr<Block> alternativeBlock = std::nullopt;

        if (this->is(TokenKind::KeywordElse)) {
            IONIR_PARSER_ASSERT(this->skipOver(TokenKind::KeywordElse))

            // The block's parent will be filled below.
            alternativeBlock = this->parseBlock(nullptr);
        }

        // Make the if statement construct.
        ionshared::Ptr<IfStatement> ifStatement = std::make_shared<IfStatement>(IfStatementOpts{
            parent,
            *condition,
            consequentBlock,
            alternativeBlock
        });

        // Finally, fill in the gaps.
        consequentBlock->setParent(ifStatement);

        if (ionshared::Util::hasValue(alternativeBlock)) {
            alternativeBlock->get()->setParent(ifStatement);
        }

        return ifStatement;
    }

    ionshared::OptPtr<ReturnStatement> Parser::parseReturnStatement(const ionshared::Ptr<Block> &parent) {
        IONIR_PARSER_ASSERT(this->skipOver(TokenKind::KeywordReturn))

        ionshared::OptPtr<Value<>> value = std::nullopt;

        // Return statement contains a value. Parse it and save it.
        if (!this->is(TokenKind::SymbolSemiColon)) {
            value = this->parseValue();

            IONIR_PARSER_ASSURE(value)
        }
        // Otherwise, simply skip the semi-colon token.
        else {
            IONIR_PARSER_ASSERT(this->skipOver(TokenKind::SymbolSemiColon))
        }

        return std::make_shared<ReturnStatement>(ReturnStatementOpts{
            parent,
            value
        });
    }
}
