#include <ionlang/lexical/classifier.h>
#include <ionlang/syntax/parser.h>
#include <ionlang/misc/util.h>
#include <ionlang/const/notice.h>

namespace ionlang {
    bool Parser::is(TokenKind tokenKind) noexcept {
        return this->tokenStream.get().getKind() == tokenKind;
    }

    bool Parser::isNext(TokenKind tokenKind) {
        return this->tokenStream.peek()->getKind() == tokenKind;
    }

    bool Parser::expect(TokenKind tokenKind) {
        if (!this->is(tokenKind)) {
            this->makeNotice("Expected token kind: " + std::to_string((int)tokenKind) + ", but got: " +
                std::to_string((int)this->tokenStream.get().getKind()));

            return false;
        }

        return true;
    }

    bool Parser::skipOver(TokenKind tokenKind) {
        if (!this->expect(tokenKind)) {
            return false;
        }

        this->tokenStream.skip();

        return true;
    }

    ionshared::NoticeFactory Parser::createNoticeFactory() noexcept {
        // Abstract current Token for easier access.
        Token token = this->tokenStream.get();

        return ionshared::NoticeFactory(ionshared::NoticeContext{
                this->filePath,
                token.getLineNumber(),
                token.getStartPosition()
            }
        );
    }

    std::nullopt_t Parser::makeNotice(std::string message, ionshared::NoticeType type) {
        this->noticeStack->push(this->createNoticeFactory().make(type, std::move(message)));

        return std::nullopt;
    }

    Parser::Parser(
        TokenStream stream,
        const ionshared::Ptr<ionshared::NoticeStack> &noticeStack,
        std::string filePath
    ) :
        tokenStream(std::move(stream)),
        noticeStack(noticeStack),
        noticeSentinel(std::make_shared<NoticeSentinel>(noticeStack)),
        filePath(std::move(filePath)) {
        //
    }

    ionshared::Ptr<NoticeSentinel> Parser::getNoticeSentinel() const {
        return this->noticeSentinel;
    }

    std::string Parser::getFilePath() const {
        return this->filePath;
    }

    AstPtrResult<> Parser::parseTopLevel(const ionshared::Ptr<Module> &parent) {
        switch (this->tokenStream.get().getKind()) {
            case TokenKind::KeywordFunction: {
                return util::getResultValue(this->parseFunction(parent));
            }

            case TokenKind::KeywordGlobal: {
                return util::getResultValue(this->parseGlobal());
            }

            case TokenKind::KeywordExtern: {
                return util::getResultValue(this->parseExtern(parent));
            }

            default: {
                // TODO: Use proper exception.
                throw std::runtime_error("Unknown top-level construct");
            }
        }
    }

    AstPtrResult<Global> Parser::parseGlobal() {
        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::KeywordGlobal), Global)

        AstPtrResult<Type> typeResult = this->parseType();

        IONLANG_PARSER_ASSERT(util::hasValue(typeResult), Global)

        std::optional<std::string> id = this->parseId();

        IONLANG_PARSER_ASSERT(id.has_value(), Global)

        AstPtrResult<Value<>> valueResult;

        // Global is being initialized inline with a value. Parse & process the value.
        if (this->is(TokenKind::SymbolEqual)) {
            // Skip the equal symbol before continuing parsing.
            this->tokenStream.skip();

            valueResult = this->parseLiteral();

            // Value must have been parsed at this point.
            if (!util::hasValue(valueResult)) {
                // TODO: Use proper exception/error.
                throw std::runtime_error("Could not parse literal");
            }
        }

        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolSemiColon), Global)

        return std::make_shared<Global>(
            util::getResultValue(typeResult),
            *id,
            util::getResultValue(valueResult)
        );
    }

    AstPtrResult<Block> Parser::parseBlock(const ionshared::Ptr<Construct> &parent) {
        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolBraceL), Block)

        ionshared::Ptr<Block> block = std::make_shared<Block>(parent);
        std::vector<ionshared::Ptr<Statement>> statements = {};

        while (!this->is(TokenKind::SymbolBraceR)) {
            AstPtrResult<Statement> statement = this->parseStatement(block);

            IONLANG_PARSER_ASSERT(util::hasValue(statement), Block)

            statements.push_back(util::getResultValue(statement));
        }

        this->tokenStream.skip();
        block->setStatements(statements);

        return block;
    }

    AstPtrResult<Module> Parser::parseModule() {
        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::KeywordModule), Module)

        std::optional<std::string> id = this->parseId();

        IONLANG_PARSER_ASSERT(id.has_value(), Module)
        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolBraceL), Module)

        Scope globalScope =
            std::make_shared<ionshared::SymbolTable<ionshared::Ptr<Construct>>>();

        ionshared::Ptr<Module> module = std::make_shared<Module>(*id, std::make_shared<Context>(globalScope));

        while (!this->is(TokenKind::SymbolBraceR)) {
            AstPtrResult<> topLevelConstructResult = this->parseTopLevel(module);

            // TODO: Make notice if it has no value? Or is it enough with the notice under 'parseTopLevel()'?
            if (util::hasValue(topLevelConstructResult)) {
                ionshared::Ptr<Construct> topLevelConstruct = util::getResultValue(topLevelConstructResult);
                std::optional<std::string> name = util::findConstructId(topLevelConstruct);

                if (!name.has_value()) {
                    throw std::runtime_error("Unexpected construct name to be null");
                }

                // TODO: Ensure we're not re-defining something, issue a notice otherwise.
                globalScope->insert(*name, topLevelConstruct);
            }

            // No more tokens to process.
            if (!this->tokenStream.hasNext() && !this->is(TokenKind::SymbolBraceR)) {
                // TODO: Use Ast(Ptr)Result<>.
                throw ionshared::util::quickError(IONLANG_NOTICE_MISC_UNEXPECTED_EOF);
                //                return this->noticeSentinel->makeError<Module>(IONIR_NOTICE_MISC_UNEXPECTED_EOF);
            }
        }

        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolBraceR), Module)

        return module;
    }

    AstPtrResult<VariableDecl> Parser::parseVariableDecl(const ionshared::Ptr<Block> &parent) {
        AstPtrResult<Type> typeResult = this->parseType();

        IONLANG_PARSER_ASSERT(util::hasValue(typeResult), VariableDecl)

        std::optional<std::string> id = this->parseId();

        IONLANG_PARSER_ASSERT(id.has_value(), VariableDecl)
        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolEqual), VariableDecl)

        AstPtrResult<Value<>> valueResult = this->parseLiteral();

        IONLANG_PARSER_ASSERT(util::hasValue(valueResult), VariableDecl)

        ionshared::Ptr<VariableDecl> variableDecl = std::make_shared<VariableDecl>(
            parent,
            util::getResultValue(typeResult),
            *id,
            util::getResultValue(valueResult)
        );

        // Register the statement on the resulting block's symbol table.
        parent->getSymbolTable()->insert(variableDecl->getId(), variableDecl);

        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolSemiColon), VariableDecl)

        return variableDecl;
    }
}
