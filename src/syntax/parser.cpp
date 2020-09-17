#include <ionlang/lexical/classifier.h>
#include <ionlang/syntax/parser.h>
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

    std::nullopt_t Parser::makeNotice(const std::string &message, ionshared::NoticeType type) {
        // TODO
        throw std::runtime_error(message);

        return std::nullopt;
    }

    ionshared::Range Parser::makeSyntaxRange() const noexcept {
        // TODO: Hard-coded 1 length.
        //            return {
        //                this->syntaxStartMarker,
        //                static_cast<uint32_t>(this->tokenStream.getIndex()) - this->syntaxStartMarker;
        //            };
        return {
            static_cast<uint32_t>(this->tokenStream.getIndex()),
            1
        };
    }

    void Parser::beginSourceLocationMapping() noexcept {
        this->sourceLocationMappingStartStack.push(std::make_pair(
            this->tokenStream.get().getLineNumber(),
            this->tokenStream.get().getStartPosition()
        ));
    }

    ionshared::SourceLocation Parser::makeSourceLocation() {
        if (this->sourceLocationMappingStartStack.empty()) {
            throw std::runtime_error("Source mapping starting point stack is empty");
        }

        std::pair<uint32_t, uint32_t> mappingStart =
            this->sourceLocationMappingStartStack.top();

        this->sourceLocationMappingStartStack.pop();

        Token currentToken = this->tokenStream.get();

        return ionshared::SourceLocation(
            ionshared::Span(
                mappingStart.first,
                currentToken.getLineNumber() - mappingStart.first
            ),

            ionshared::Span(
                mappingStart.second,
                currentToken.getEndPosition() - mappingStart.second
            )
        );
    }

    void Parser::mapSourceLocation(AstPtrResult<> construct) {
        // TODO: Causing error regarding std::pair.
//        this->sourceMap->set(
//            util::getResultValue(std::move(construct)),
//            this->makeSourceLocation()
//        );
    }

    void Parser::finishSourceLocationMapping(const ionshared::Ptr<Construct> &construct) {
        construct->setSourceLocation(this->makeSourceLocation());
    }

    Parser::Parser(
        TokenStream stream,
        const ionshared::Ptr<ionshared::NoticeStack> &noticeStack,
        std::string filePath
    ) :
        tokenStream(std::move(stream)),
        noticeStack(noticeStack),
        noticeSentinel(std::make_shared<NoticeSentinel>(noticeStack)),
        filePath(std::move(filePath)),
        sourceLocationMappingStartStack() {
        //
    }

    ionshared::Ptr<NoticeSentinel> Parser::getNoticeSentinel() const {
        return this->noticeSentinel;
    }

    std::string Parser::getFilePath() const {
        return this->filePath;
    }

    AstPtrResult<> Parser::parseTopLevelFork(const ionshared::Ptr<Module> &parent) {
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
        this->beginSourceLocationMapping();

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

            valueResult = this->parseLiteralFork();

            // Value must have been parsed at this point.
            if (!util::hasValue(valueResult)) {
                // TODO: Use proper exception/error.
                throw std::runtime_error("Could not parse literal");
            }
        }

        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolSemiColon), Global)

        ionshared::Ptr<Global> global = std::make_shared<Global>(
            util::getResultValue(typeResult),
            *id,
            util::getResultValue(valueResult)
        );

        this->finishSourceLocationMapping(global);

        return global;
    }

    AstPtrResult<Block> Parser::parseBlock(const ionshared::Ptr<Construct> &parent) {
        this->beginSourceLocationMapping();
        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolBraceL), Block)

        ionshared::Ptr<Block> block = std::make_shared<Block>(parent);

        while (!this->is(TokenKind::SymbolBraceR)) {
            AstPtrResult<Statement> statement = this->parseStatement(block);

            IONLANG_PARSER_ASSERT(util::hasValue(statement), Block)

            block->appendStatement(util::getResultValue(statement));
        }

        this->tokenStream.skip();

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
            AstPtrResult<> topLevelConstructResult = this->parseTopLevelFork(module);

            // TODO: Make notice if it has no value? Or is it enough with the notice under 'parseTopLevel()'?
            if (util::hasValue(topLevelConstructResult)) {
                ionshared::Ptr<Construct> topLevelConstruct = util::getResultValue(topLevelConstructResult);
                std::optional<std::string> name = util::findConstructId(topLevelConstruct);

                if (!name.has_value()) {
                    throw std::runtime_error("Unexpected construct name to be null");
                }

                // TODO: Ensure we're not re-defining something, issue a notice otherwise.
                globalScope->set(*name, topLevelConstruct);
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

    AstPtrResult<VariableDeclStatement> Parser::parseVariableDecl(const ionshared::Ptr<Block> &parent) {
        AstPtrResult<Type> typeResult = this->parseType();

        IONLANG_PARSER_ASSERT(util::hasValue(typeResult), VariableDeclStatement)

        std::optional<std::string> id = this->parseId();

        IONLANG_PARSER_ASSERT(id.has_value(), VariableDeclStatement)
        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolEqual), VariableDeclStatement)

        AstPtrResult<Value<>> valueResult = this->parseLiteralFork();

        IONLANG_PARSER_ASSERT(util::hasValue(valueResult), VariableDeclStatement)

        ionshared::Ptr<VariableDeclStatement> variableDecl = std::make_shared<VariableDeclStatement>(VariableDeclStatementOpts{
            parent,
            util::getResultValue(typeResult),
            *id,
            util::getResultValue(valueResult)
        });

        // Register the statement on the resulting block's symbol table.
        parent->getSymbolTable()->set(variableDecl->getId(), variableDecl);

        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolSemiColon), VariableDeclStatement)

        return variableDecl;
    }
}
