#include <ionlang/const/const.h>
#include <ionlang/lexical/classifier.h>
#include <ionlang/syntax/parser.h>

namespace ionlang {
    bool Parser::is(TokenKind tokenKind) noexcept {
        return this->tokenStream.get().kind == tokenKind;
    }

    bool Parser::isNext(TokenKind tokenKind) {
        return this->tokenStream.peek()->kind == tokenKind;
    }

    bool Parser::expect(TokenKind tokenKind) {
        if (this->is(tokenKind)) {
            return true;
        }

        this->diagnosticBuilder
            ->bootstrap(diagnostic::syntaxExpectedOtherToken)
            ->setSourceLocation(this->makeSourceLocation())
            ->formatMessage(
                Grammar::findTokenKindNameOr(tokenKind),
                Grammar::findTokenKindNameOr(this->tokenStream.get().kind)
            )
            ->finish();

        return false;
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

    std::shared_ptr<ErrorMarker> Parser::makeErrorMarker() {
        std::shared_ptr<ErrorMarker> errorMarker = std::make_shared<ErrorMarker>();

        errorMarker->sourceLocation = this->makeSourceLocation();

        return errorMarker;
    }

    void Parser::finishSourceLocationMapping(const std::shared_ptr<Construct>& construct) {
        construct->sourceLocation = this->finishSourceLocation();
    }

    Parser::Parser(
        TokenStream stream,
        std::shared_ptr<ionshared::DiagnosticBuilder> diagnosticBuilder
    ) noexcept :
        moduleBuffer(std::nullopt),
        tokenStream(std::move(stream)),
        diagnosticBuilder(std::move(diagnosticBuilder)),
        sourceLocationMappingStartStack() {
        //
    }

    std::shared_ptr<ionshared::DiagnosticBuilder> Parser::getDiagnosticBuilder() const noexcept {
        return this->diagnosticBuilder;
    }

    AstPtrResult<> Parser::parseTopLevelConstruct(const std::shared_ptr<Module>& parent) {
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
                return util::getResultValue(this->parseStructType(parent));
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

    AstPtrResult<Global> Parser::parseGlobal(const std::shared_ptr<Module>& parent) {
        this->beginSourceLocationMapping();
        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::KeywordGlobal))

        // TODO: Not proper parent.
        AstPtrResult<Resolvable<Type>> typeResult = this->parseType(parent);

        IONLANG_PARSER_ASSERT(util::hasValue(typeResult))

        std::optional<std::string> name = this->parseName();

        IONLANG_PARSER_ASSERT(name.has_value())

        AstPtrResult<Expression<>> valueResult;

        // Global is being initialized inline with a value. Parse & process the value.
        if (this->is(TokenKind::SymbolEqual)) {
            // Skip the equal symbol before continuing parsing.
            this->tokenStream.skip();

            // TODO: Not proper parent.
            // TODO: What about full expressions on globals?
            valueResult = this->parseLiteral(parent);

            IONLANG_PARSER_ASSERT(util::hasValue(valueResult))
        }

        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolSemiColon))

        std::shared_ptr<Global> global = Global::make(
            util::getResultValue(typeResult),
            *name,
            util::getResultValue(valueResult)
        );

        global->parent = parent;
        this->finishSourceLocationMapping(global);

        return global;
    }

    AstPtrResult<StructType> Parser::parseStructType(const std::shared_ptr<Module>& parent) {
        this->beginSourceLocationMapping();

        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::KeywordStruct))

        std::optional<std::string> structNameResult = this->parseName();

        IONLANG_PARSER_ASSERT(structNameResult.has_value())
        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolBraceL))

        Fields fields = ionshared::util::makePtrSymbolTable<Resolvable<Type>>();

        ionshared::PtrSymbolTable<Method> methods =
            ionshared::util::makePtrSymbolTable<Method>();

        TokenKind currentTokenKind = this->tokenStream.get().kind;

        while (!this->is(TokenKind::SymbolBraceR)) {
            // Field.
            if (Classifier::isBuiltInType(currentTokenKind)
                || this->is(TokenKind::Identifier)) {
                // TODO: Not proper parent.
                AstPtrResult<Resolvable<Type>> fieldTypeResult = this->parseType(parent);

                IONLANG_PARSER_ASSERT(util::hasValue(fieldTypeResult))

                std::optional<std::string> fieldNameResult = this->parseName();

                IONLANG_PARSER_ASSERT(fieldNameResult.has_value())
                IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolSemiColon))

                /**
                 * A field with the same name was already previously
                 * parsed and set on the fields map.
                 */
                if (fields->contains(*fieldNameResult)) {
                    this->diagnosticBuilder
                        ->bootstrap(diagnostic::structFieldRedefinition)
                        ->formatMessage(*fieldNameResult, *structNameResult)
                        ->setSourceLocation(this->makeSourceLocation())
                        ->finish();

                    return this->makeErrorMarker();
                }

                fields->set(*fieldNameResult, util::getResultValue(fieldTypeResult));
            }
            // Method.
            else if (Classifier::isMethodOrFunction(currentTokenKind)) {
                // TODO: Parent is nullptr, should be 'structType' (below).
                AstPtrResult<Method> methodResult = this->parseMethod(nullptr);

                IONLANG_PARSER_ASSERT(util::hasValue(methodResult))

                std::shared_ptr<Method> method = util::getResultValue(methodResult);

                /**
                 * A field with the same name was already previously
                 * parsed and set on the fields map.
                 */
                if (methods->contains(method->prototype->name)) {
                    this->diagnosticBuilder
                        ->bootstrap(diagnostic::structMethodRedefinition)
                        ->formatMessage(method->prototype->name, *structNameResult)
                        ->setSourceLocation(method->sourceLocation)
                        ->finish();

                    return this->makeErrorMarker();
                }
            }

            // TODO: What if reached here?
        }

        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolBraceR))

        std::shared_ptr<StructType> structType = StructType::make(
            *structNameResult,
            fields,
            methods
        );

        structType->parent = parent;

        return structType;
    }

    AstPtrResult<Block> Parser::parseBlock(const std::shared_ptr<Construct>& parent) {
        this->beginSourceLocationMapping();
        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolBraceL))

        std::shared_ptr<Block> block = Block::make();

        block->parent = parent;

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

        std::optional<std::string> id = this->parseName();

        IONLANG_PARSER_ASSERT(id.has_value())
        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolBraceL))

        Context::Scope globalScope =
            ionshared::util::makePtrSymbolTable<Construct>();

        std::shared_ptr<Module> module =
            std::make_shared<Module>(*id, std::make_shared<Context>(globalScope));

        this->moduleBuffer = module;

        while (!this->is(TokenKind::SymbolBraceR)) {
            AstPtrResult<> topLevelConstructResult = this->parseTopLevelConstruct(module);

            // TODO: Make notice if it has no value? Or is it enough with the notice under 'parseTopLevel()'?
            if (util::hasValue(topLevelConstructResult)) {
                std::shared_ptr<Construct> topLevelConstruct = util::getResultValue(topLevelConstructResult);
                std::optional<std::string> name = util::findConstructId(topLevelConstruct);

                IONLANG_PARSER_ASSERT(name.has_value())

                // TODO: Ensure we're not re-defining something, issue a notice otherwise.
                globalScope->set(*name, topLevelConstruct);
            }

            // No more tokens to process.
            if (!this->tokenStream.hasNext() && !this->is(TokenKind::SymbolBraceR)) {
                this->getDiagnosticBuilder()
                    ->bootstrap(diagnostic::syntaxExpectedOtherToken)

                    ->formatMessage(
                        Grammar::findTokenKindNameOr(TokenKind::SymbolBraceR),
                        Grammar::findTokenKindNameOr(this->tokenStream.get().kind)
                    )

                    ->setSourceLocation(this->makeSourceLocation())
                    ->finish();

                return this->makeErrorMarker();
            }
        }

        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolBraceR))
        this->finishSourceLocationMapping(module);

        return module;
    }

    AstPtrResult<Identifier> Parser::parseIdentifier() {
        std::string baseName{};
        std::vector<std::string> scopePath{};
        bool isPrime = true;

        this->beginSourceLocationMapping();

        do {
            if (this->is(TokenKind::SymbolScope) && isPrime) {
                this->getDiagnosticBuilder()
                    ->bootstrap(diagnostic::syntaxExpectedOtherToken)

                    ->formatMessage(
                        Grammar::findTokenKindNameOr(TokenKind::Identifier),
                        Grammar::findTokenKindNameOr(TokenKind::SymbolScope)
                    )

                    ->setSourceLocation(this->makeSourceLocation())
                    ->finish();

                return this->makeErrorMarker();
            }
            else if (!isPrime) {
                IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolScope));
            }

            std::optional<std::string> name = this->parseName();

            IONLANG_PARSER_ASSERT(name.has_value())

            if (isPrime) {
                baseName = *name;
            }
            else {
                scopePath.push_back(*name);
            }

            isPrime = false;

        }
        while (this->is(TokenKind::Identifier)
            || this->is(TokenKind::SymbolScope));

        std::shared_ptr<Identifier> id =
            std::make_shared<Identifier>(baseName, scopePath);

        this->finishSourceLocationMapping(id);

        return id;
    }

    AstPtrResult<Import> Parser::parseImport() {
        this->beginSourceLocationMapping();
        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::KeywordImport));

        AstPtrResult<Identifier> id = this->parseIdentifier();

        IONLANG_PARSER_ASSERT(util::hasValue(id))
        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolSemiColon))

        std::shared_ptr<Import> import =
            Import::make(util::getResultValue(id));

        this->finishSourceLocationMapping(import);

        return import;
    }

    AstPtrResult<> Parser::parseIntrinsic(const std::shared_ptr<Block>& parent) {
        this->beginSourceLocationMapping();
        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::KeywordIntrinsic))
        IONLANG_PARSER_ASSERT(this->skipOver(TokenKind::SymbolScope))

        AstPtrResult<CallExpr> callExpr = this->parseCallExpr(parent);

        IONLANG_PARSER_ASSERT(util::hasValue(callExpr))

        std::optional<std::shared_ptr<Identifier>> calleeResolvableId =
            util::getResultValue(callExpr)->calleeResolvable->id;

        IONLANG_PARSER_ASSERT(calleeResolvableId.has_value())
        IONLANG_PARSER_ASSERT(calleeResolvableId->get()->scopePath.size() == 1)

        std::string intrinsicModuleName = calleeResolvableId->get()->baseName;

        if (!Const::intrinsicModules.contains(intrinsicModuleName)) {
            this->getDiagnosticBuilder()
                ->bootstrap(diagnostic::intrinsicUnknownModule)
                ->formatMessage(intrinsicModuleName)
                ->setSourceLocation(this->makeSourceLocation())
                ->finish();

            return this->makeErrorMarker();
        }

        IntrinsicModuleKind intrinsicModuleKind =
            Const::intrinsicModules.unwrapConst().at(intrinsicModuleName);

        std::string calleeName = calleeResolvableId->get()->scopePath.at(0);

        switch (intrinsicModuleKind) {
            case IntrinsicModuleKind::Reflection: {
                // TODO: Hard-coded strings.

                if (calleeName == "nameOf") {
                    // TODO: Access CallExpr's args.
//                    return std::make_shared<NameOfIntrinsic>();
                    throw std::runtime_error("Not implemented");
                }
                else if (calleeName == "typeOf") {
                    // TODO: Access CallExpr's args.
//                    return std::make_shared<TypeOfIntrinsic>();
                    throw std::runtime_error("Not implemented");
                }

                // TODO: Use diagnostics API (user error).
                throw std::runtime_error("Unknown intrinsic callee name");
            }

            default: {
                // TODO: Use diagnostics API (internal error).
                throw std::runtime_error("Unknown intrinsic module kind");
            }
        }
    }

    AstPtrResult<Method> Parser::parseMethod(const std::shared_ptr<StructType>& structType) {
        this->beginSourceLocationMapping();

        TokenKind currentTokenKind = this->tokenStream.get().kind;
        MethodKind methodKind;

        switch (currentTokenKind) {
            case TokenKind::KeywordFunction: {
                methodKind = MethodKind::Normal;

                break;
            }

            case TokenKind::KeywordOperator: {
                methodKind = MethodKind::OperatorOverload;

                break;
            }

            case TokenKind::KeywordConstructor: {
                methodKind = MethodKind::Constructor;

                break;
            }

            case TokenKind::KeywordDestructor: {
                methodKind = MethodKind::Destructor;

                break;
            }

            default: {
                this->diagnosticBuilder
                    ->bootstrap(diagnostic::syntaxUnexpectedToken_)
                    ->formatMessage(Grammar::findTokenKindNameOr(currentTokenKind))
                    ->setSourceLocation(this->makeSourceLocation())
                    ->finish();

                return this->makeErrorMarker();
            }
        }

        this->tokenStream.skip();

        // TODO: CRITICAL x2! Parent should be 'method' (below).
        // TODO: Parsing prototype WILL fail on constructors/destructors, as well as all others except function/Normal (they don't have fn keyword).
        AstPtrResult<Prototype> prototypeResult = this->parsePrototype(structType);

        IONLANG_PARSER_ASSERT(util::hasValue(prototypeResult))

        // TODO: CRITICAL x2! CRITICAL x3! Parent should be 'method' (below). Reason CRITICAL is that block emitting (might) depend on parent.
        AstPtrResult<Block> bodyResult = this->parseBlock(structType);

        IONLANG_PARSER_ASSERT(util::hasValue(bodyResult))

        /**
         * Create the resulting function construct here, to be provided
         * as the parent when parsing the body block.
         */
        std::shared_ptr<Method> method = Method::make(
            methodKind,
            structType,
            util::getResultValue(prototypeResult),
            util::getResultValue(bodyResult)
        );

        method->parent = structType;
        this->finishSourceLocationMapping(method);

        return method;
    }
}
