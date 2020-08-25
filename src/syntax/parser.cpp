#include <ionlang/lexical/classifier.h>
#include <ionlang/syntax/parser.h>
#include <ionlang/misc/util.h>
#include <ionlang/const/notice.h>

namespace ionlang {
    bool Parser::is(TokenKind tokenKind) noexcept {
        return this->stream.get().getKind() == tokenKind;
    }

    bool Parser::isNext(TokenKind tokenKind) {
        return this->stream.peek()->getKind() == tokenKind;
    }

    bool Parser::expect(TokenKind tokenKind) {
        if (!this->is(tokenKind)) {
            this->makeNotice("Expected token kind: " + std::to_string((int)tokenKind) + ", but got: " +
                std::to_string((int)this->stream.get().getKind()));

            return false;
        }

        return true;
    }

    bool Parser::skipOver(TokenKind tokenKind) {
        if (!this->expect(tokenKind)) {
            return false;
        }

        this->stream.skip();

        return true;
    }

    ionshared::NoticeFactory Parser::createNoticeFactory() noexcept {
        // Abstract current Token for easier access.
        Token token = this->stream.get();

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

    Parser::Parser(TokenStream stream, const ionshared::Ptr<ionshared::NoticeStack> &noticeStack, std::string filePath)
        : stream(std::move(stream)), noticeStack(noticeStack),
        noticeSentinel(std::make_shared<NoticeSentinel>(noticeStack)), filePath(std::move(filePath)) {
        //
    }

    ionshared::Ptr<NoticeSentinel> Parser::getNoticeSentinel() const {
        return this->noticeSentinel;
    }

    std::string Parser::getFilePath() const {
        return this->filePath;
    }

    ionshared::OptPtr<Construct> Parser::parseTopLevel(const ionshared::Ptr<Module> &parent) {
        switch (this->stream.get().getKind()) {
            case TokenKind::KeywordFunction: {
                return this->parseFunction(parent);
            }

            case TokenKind::KeywordGlobal: {
                return this->parseGlobal();
            }

            case TokenKind::KeywordExtern: {
                return this->parseExtern(parent);
            }

            default: {
                return this->makeNotice("Unknown top-level construct");
            }
        }
    }

    ionshared::OptPtr<Global> Parser::parseGlobal() {
        IONIR_PARSER_ASSERT(this->skipOver(TokenKind::KeywordGlobal))

        ionshared::OptPtr<Type> type = this->parseType();

        IONIR_PARSER_ASSURE(type)

        std::optional<std::string> id = this->parseId();

        IONIR_PARSER_ASSURE(id)

        ionshared::OptPtr<Value<>> value = std::nullopt;

        // Global is being initialized inline with a value. Parse & process the value.
        if (this->is(TokenKind::SymbolEqual)) {
            // Skip the equal symbol before continuing parsing.
            this->stream.skip();

            value = this->parseValue();

            // Value must have been parsed at this point.
            if (!ionshared::Util::hasValue(value)) {
                return std::nullopt;
            }
        }

        IONIR_PARSER_ASSERT(this->skipOver(TokenKind::SymbolSemiColon))

        return std::make_shared<Global>(*type, *id, value);
    }

    ionshared::OptPtr<Block> Parser::parseBlock(const ionshared::Ptr<Construct> &parent) {
        IONIR_PARSER_ASSERT(this->skipOver(TokenKind::SymbolBraceL))

        ionshared::Ptr<Block> block = std::make_shared<Block>(parent);
        std::vector<ionshared::Ptr<Statement>> statements = {};

        while (!this->is(TokenKind::SymbolBraceR)) {
            ionshared::OptPtr<Statement> statement = this->parseStatement(block);

            IONIR_PARSER_ASSURE(statement)

            statements.push_back(*statement);
        }

        this->stream.skip();
        block->setStatements(statements);

        return block;
    }

    ionshared::OptPtr<Module> Parser::parseModule() {
        IONIR_PARSER_ASSERT(this->skipOver(TokenKind::KeywordModule))

        std::optional<std::string> id = this->parseId();

        IONIR_PARSER_ASSURE(id)
        IONIR_PARSER_ASSERT(this->skipOver(TokenKind::SymbolBraceL))

        ionshared::PtrSymbolTable<Construct> symbolTable =
            std::make_shared<ionshared::SymbolTable<ionshared::Ptr<Construct>>>();

        ionshared::Ptr<Module> module = std::make_shared<Module>(*id, symbolTable);

        while (!this->is(TokenKind::SymbolBraceR)) {
            ionshared::OptPtr<Construct> topLevelConstructResult = this->parseTopLevel(module);

            // TODO: Make notice if it has no value? Or is it enough with the notice under 'parseTopLevel()'?
            if (ionshared::Util::hasValue(topLevelConstructResult)) {
                ionshared::Ptr<Construct> topLevelConstruct = *topLevelConstructResult;
                std::optional<std::string> name = Util::findConstructId(topLevelConstruct);

                if (!name.has_value()) {
                    throw std::runtime_error("Unexpected construct name to be null");
                }

                // TODO: Ensure we're not re-defining something, issue a notice otherwise.
                symbolTable->insert(*name, topLevelConstruct);
            }

            // No more tokens to process.
            if (!this->stream.hasNext() && !this->is(TokenKind::SymbolBraceR)) {
                // TODO: Use Ast(Ptr)Result<>.
                throw ionshared::Util::quickError(IONLANG_NOTICE_MISC_UNEXPECTED_EOF);
                //                return this->noticeSentinel->makeError<Module>(IONIR_NOTICE_MISC_UNEXPECTED_EOF);
            }
        }

        IONIR_PARSER_ASSERT(this->skipOver(TokenKind::SymbolBraceR))

        return module;
    }

    ionshared::OptPtr<VariableDecl> Parser::parseVariableDecl(const ionshared::Ptr<Block> &parent) {
        ionshared::OptPtr<Type> type = this->parseType();

        IONIR_PARSER_ASSURE(type)

        std::optional<std::string> id = this->parseId();

        IONIR_PARSER_ASSURE(id)
        IONIR_PARSER_ASSERT(this->skipOver(TokenKind::SymbolEqual))

        ionshared::OptPtr<Value<>> value = this->parseValue();

        IONIR_PARSER_ASSURE(value)

        ionshared::Ptr<VariableDecl> variableDecl =
            std::make_shared<VariableDecl>(parent, *type, *id, *value);

        // Register the statement on the resulting block's symbol table.
        parent->getSymbolTable()->insert(variableDecl->getId(), variableDecl);

        IONIR_PARSER_ASSERT(this->skipOver(TokenKind::SymbolSemiColon))

        return variableDecl;
    }
}
