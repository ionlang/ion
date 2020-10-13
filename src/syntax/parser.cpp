#include <ionlang/lexical/classifier.h>
#include <ionlang/const/notice.h>
#include <ionlang/const/const_name.h>
#include <ionlang/syntax/parser.h>

namespace ionlang {
    bool Parser::is(TokenKind tokenKind) noexcept {
        return this->tokenStream.get().kind == tokenKind;
    }

    bool Parser::isNext(TokenKind tokenKind) {
        return this->tokenStream.peek()->kind == tokenKind;
    }

    bool Parser::expect(TokenKind tokenKind) {
        if (!this->is(tokenKind)) {
            std::string expectedTokenKindName = const_name::unknown;
            std::string actualTokenKindName = const_name::unknown;
            TokenKind currentTokenKind = this->tokenStream.get().kind;

            if (Grammar::tokenKindNames.contains(tokenKind)) {
                expectedTokenKindName = Grammar::tokenKindNames.at(tokenKind);
            }

            if (Grammar::tokenKindNames.contains(currentTokenKind)) {
                expectedTokenKindName = Grammar::tokenKindNames.at(currentTokenKind);
            }

            this->diagnosticBuilder
                ->bootstrap(diagnostic::syntaxExpectedToken)
                ->setSourceLocation(this->makeSourceLocation())
                ->formatMessage(expectedTokenKindName, actualTokenKindName)
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
            this->tokenStream.get().lineNumber,
            this->tokenStream.get().startPosition
        ));
    }

    ionshared::SourceLocation Parser::makeSourceLocation() {
        if (this->sourceLocationMappingStartStack.empty()) {
            throw std::runtime_error("Source mapping starting point stack is empty");
        }

        std::pair<uint32_t, uint32_t> mappingStart =
            this->sourceLocationMappingStartStack.top();

        Token currentToken = this->tokenStream.get();

        return ionshared::SourceLocation{
            ionshared::Span{
                mappingStart.first,
                currentToken.lineNumber - mappingStart.first
            },

            ionshared::Span{
                mappingStart.second,
                currentToken.getEndPosition() - mappingStart.second
            }
        };
    }

    ionshared::SourceLocation Parser::finishSourceLocation() {
        ionshared::SourceLocation sourceLocation = this->makeSourceLocation();

        this->sourceLocationMappingStartStack.pop();

        return sourceLocation;
    }

    ionshared::Ptr<ErrorMarker> Parser::makeErrorMarker() {
        ionshared::Ptr<ErrorMarker> errorMarker = std::make_shared<ErrorMarker>();

        errorMarker->sourceLocation = this->makeSourceLocation();

        return errorMarker;
    }

    void Parser::mapSourceLocation(const AstPtrResult<>& construct) {
        // TODO: Causing error regarding std::pair.
//        this->sourceMap->set(
//            util::getResultValue(std::move(construct)),
//            this->makeSourceLocation()
//        );
    }

    void Parser::finishSourceLocationMapping(const ionshared::Ptr<Construct>& construct) {
        construct->sourceLocation = this->finishSourceLocation();
    }

    Parser::Parser(
        TokenStream stream,
        ionshared::Ptr<ionshared::DiagnosticBuilder> diagnosticBuilder
    ) noexcept :
        tokenStream(std::move(stream)),
        diagnosticBuilder(std::move(diagnosticBuilder)),
        sourceLocationMappingStartStack() {
        //
    }

    ionshared::Ptr<ionshared::DiagnosticBuilder> Parser::getDiagnosticBuilder() const noexcept {
        return this->diagnosticBuilder;
    }

    AstPtrResult<> Parser::parseTopLevelFork(const ionshared::Ptr<Module>& parent) {
        this->beginSourceLocationMapping();

        switch (this->tokenStream.get().kind) {
            case TokenKind::KeywordFunction: {
                return util::getResultValue(this->parseFunction(parent));
            }

            case TokenKind::KeywordGlobal: {
                return util::getResultValue(this->parseGlobal(parent));
            }

            case TokenKind::KeywordExtern: {
                return util::getResultValue(this->parseExtern(parent));
            }

            case TokenKind::KeywordStruct: {
                return util::getResultValue(this->parseStruct(parent));
            }

            default: {
                this->diagnosticBuilder
                    ->bootstrap(diagnostic::internalUnexpectedToken)
                    ->setSourceLocation(this->makeSourceLocation())
                    ->finish();

                return this->makeErrorMarker();
            }
        }
    }

    AstPtrResult<Global> Parser::parseGlobal(const ionshared::Ptr<Module>& parent) {
        this->beginSourceLocationMapping();

        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::KeywordGlobal))

        AstPtrResult<Type> typeResult = this->parseType();

        IONLANG_PARSER_ASSERT(util::hasValue(typeResult))

        std::optional<std::string> id = this->parseId();

        IONLANG_PARSER_ASSERT(id.has_value())

        AstPtrResult<Expression<>> valueResult;

        // Global is being initialized inline with a value. Parse & process the value.
        if (this->is(TokenKind::SymbolEqual)) {
            // Skip the equal symbol before continuing parsing.
            this->tokenStream.skip();

            valueResult = this->parseLiteral();

            IONLANG_PARSER_ASSERT(util::hasValue(valueResult))
        }

        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolSemiColon))

        ionshared::Ptr<Global> global = std::make_shared<Global>(
            parent,
            util::getResultValue(typeResult),
            *id,
            util::getResultValue(valueResult)
        );

        this->finishSourceLocationMapping(global);

        return global;
    }

    AstPtrResult<Struct> Parser::parseStruct(const ionshared::Ptr<Module>& parent) {
        this->beginSourceLocationMapping();

        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::KeywordStruct))

        std::optional<std::string> structNameResult = this->parseId();

        IONLANG_PARSER_ASSERT(structNameResult.has_value())
        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolBraceL))

        Fields fields = ionshared::util::makePtrSymbolTable<Type>();

        while (!this->is(TokenKind::SymbolBraceR)) {
            AstPtrResult<Type> fieldTypeResult = this->parseType();

            IONLANG_PARSER_ASSERT(util::hasValue(fieldTypeResult))

            std::optional<std::string> fieldNameResult = this->parseId();

            IONLANG_PARSER_ASSERT(fieldNameResult.has_value())
            IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolSemiColon))

            /**
             * A field with the same name was already previously
             * parsed and set on the fields map.
             */
            if (fields->contains(*fieldNameResult)) {
                // TODO: Replace strings, since 'structFieldRedefinition' takes in field and struct names.
                this->diagnosticBuilder
                    ->bootstrap(diagnostic::structFieldRedefinition)
                    ->setSourceLocation(this->makeSourceLocation())
                    ->finish();

                return this->makeErrorMarker();
            }

            fields->set(*fieldNameResult, util::getResultValue(fieldTypeResult));
        }

        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolBraceR))

        return std::make_shared<Struct>(parent, *structNameResult, fields);
    }

    AstPtrResult<Block> Parser::parseBlock(const ionshared::Ptr<Construct>& parent) {
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
        // TODO: This should be present anywhere IONLANG_PARSER_ASSERT is used, because it invokes the finalizer.
        this->beginSourceLocationMapping();

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

        this->finishSourceLocationMapping(module);

        return module;
    }

    AstPtrResult<VariableDeclStatement> Parser::parseVariableDecl(const ionshared::Ptr<Block>& parent) {
        this->beginSourceLocationMapping();

        AstPtrResult<Type> typeResult = this->parseType();

        IONLANG_PARSER_ASSERT(util::hasValue(typeResult))

        std::optional<std::string> id = this->parseId();

        IONLANG_PARSER_ASSERT(id.has_value())
        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolEqual))

        AstPtrResult<Expression<>> valueResult = this->parseLiteral();

        IONLANG_PARSER_ASSERT(util::hasValue(valueResult))

        ionshared::Ptr<VariableDeclStatement> variableDecl = std::make_shared<VariableDeclStatement>(VariableDeclStatementOpts{
            parent,
            util::getResultValue(typeResult),
            *id,
            util::getResultValue(valueResult)
        });

        // Register the statement on the resulting block's symbol table.
        parent->symbolTable->set(variableDecl->name, variableDecl);

        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolSemiColon))

        this->finishSourceLocationMapping(variableDecl);

        return variableDecl;
    }
}
