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
            this->diagnosticBuilder
                ->beginError("Expected token kind: " + std::to_string((int)tokenKind) + ", but got: " +
                    std::to_string((int)this->tokenStream.get().getKind()))

                ->finish();

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

    ionshared::SourceLocation Parser::finishSourceLocation() {
        ionshared::SourceLocation sourceLocation = this->makeSourceLocation();

        this->sourceLocationMappingStartStack.pop();

        return sourceLocation;
    }

    ionshared::Ptr<ErrorMarker> Parser::makeErrorMarker() {
        ionshared::Ptr<ErrorMarker> errorMarker = std::make_shared<ErrorMarker>();

        errorMarker->setSourceLocation(this->makeSourceLocation());

        return errorMarker;
    }

    void Parser::mapSourceLocation(const AstPtrResult<> &construct) {
        // TODO: Causing error regarding std::pair.
//        this->sourceMap->set(
//            util::getResultValue(std::move(construct)),
//            this->makeSourceLocation()
//        );
    }

    void Parser::finishSourceLocationMapping(const ionshared::Ptr<Construct> &construct) {
        construct->setSourceLocation(this->finishSourceLocation());
    }

    Parser::Parser(
        TokenStream stream,
        ionshared::Ptr<ionshared::DiagnosticBuilder> diagnosticBuilder
    ) :
        tokenStream(std::move(stream)),
        diagnosticBuilder(std::move(diagnosticBuilder)),
        sourceLocationMappingStartStack() {
        //
    }

    ionshared::Ptr<ionshared::DiagnosticBuilder> Parser::getDiagnosticBuilder() const {
        return this->diagnosticBuilder;
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

        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::KeywordGlobal))

        AstPtrResult<Type> typeResult = this->parseType();

        IONLANG_PARSER_ASSERT(util::hasValue(typeResult))

        std::optional<std::string> id = this->parseId();

        IONLANG_PARSER_ASSERT(id.has_value())

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

        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolSemiColon))

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
        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolBraceL))

        ionshared::Ptr<Block> block = std::make_shared<Block>(parent);

        while (!this->is(TokenKind::SymbolBraceR)) {
            AstPtrResult<Statement> statement = this->parseStatement(block);

            IONLANG_PARSER_ASSERT(util::hasValue(statement))

            block->appendStatement(util::getResultValue(statement));
        }

        this->tokenStream.skip();

        return block;
    }

    AstPtrResult<Module> Parser::parseModule() {
        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::KeywordModule))

        std::optional<std::string> id = this->parseId();

        IONLANG_PARSER_ASSERT(id.has_value())
        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolBraceL))

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

        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolBraceR))

        return module;
    }

    AstPtrResult<VariableDeclStatement> Parser::parseVariableDecl(const ionshared::Ptr<Block> &parent) {
        AstPtrResult<Type> typeResult = this->parseType();

        IONLANG_PARSER_ASSERT(util::hasValue(typeResult))

        std::optional<std::string> id = this->parseId();

        IONLANG_PARSER_ASSERT(id.has_value())
        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolEqual))

        AstPtrResult<Value<>> valueResult = this->parseLiteralFork();

        IONLANG_PARSER_ASSERT(util::hasValue(valueResult))

        ionshared::Ptr<VariableDeclStatement> variableDecl = std::make_shared<VariableDeclStatement>(VariableDeclStatementOpts{
            parent,
            util::getResultValue(typeResult),
            *id,
            util::getResultValue(valueResult)
        });

        // Register the statement on the resulting block's symbol table.
        parent->getSymbolTable()->set(variableDecl->getId(), variableDecl);

        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolSemiColon))

        return variableDecl;
    }
}
