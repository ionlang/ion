#include <ionshared/misc/util.h>
#include <ionir/construct/value/integer_literal.h>
#include <ionir/construct/value/char_literal.h>
#include <ionir/construct/value/string_literal.h>
#include <ionir/construct/value/boolean_literal.h>
#include <ionir/construct/function.h>
#include <ionir/construct/global.h>
#include <ionir/construct/extern.h>
#include <ionir/construct/identifier.h>
#include <ionir/construct/struct.h>
#include <ionir/construct/struct_definition.h>
#include <ionir/misc/inst_builder.h>
#include <ionir/const/const.h>
#include <ionlang/construct/statement/return_statement.h>
#include <ionlang/passes/lowering/ionir_lowering_pass.h>
#include <ionlang/const/notice.h>
#include <ionlang/const/const.h>
#include <ionlang/misc/util.h>

namespace ionlang {
    ionshared::Ptr<ionir::Type> IonIrLoweringPass::processTypeQualifiers(
        ionshared::Ptr<ionir::Type> type,
        const ionshared::Ptr<TypeQualifiers>& qualifiers
    ) {
        ionshared::Ptr<ionir::TypeQualifiers> ionIrTypeQualifiers =
            std::make_shared<ionir::TypeQualifiers>();

        auto typeQualifiersSet = qualifiers->unwrap();

        for (const auto &typeQualifier : typeQualifiersSet) {
            ionir::TypeQualifier ionIrTypeQualifier;

            switch (typeQualifier) {
                case TypeQualifier::Pointer: {
                    ionIrTypeQualifier = ionir::TypeQualifier::Pointer;

                    break;
                }

                case TypeQualifier::Reference: {
                    ionIrTypeQualifier = ionir::TypeQualifier::Reference;

                    break;
                }

                case TypeQualifier::Constant: {
                    ionIrTypeQualifier = ionir::TypeQualifier::Constant;

                    break;
                }

                case TypeQualifier::Mutable: {
                    ionIrTypeQualifier = ionir::TypeQualifier::Mutable;

                    break;
                }

                default: {
                    throw std::runtime_error("Unknown type qualifier");
                }
            }

            ionIrTypeQualifiers->add(ionIrTypeQualifier);
        }

        type->qualifiers = ionIrTypeQualifiers;

        return type;
    }

    ionshared::Ptr<ionir::Module> IonIrLoweringPass::requireModule() {
        if (!ionshared::util::hasValue(this->buffers.module)) {
            throw std::runtime_error("Expected the module buffer to be set, but was nullptr");
        }

        return *this->buffers.module;
    }

    ionshared::Ptr<ionir::Function> IonIrLoweringPass::requireFunction() {
        if (!ionshared::util::hasValue(this->buffers.function)) {
            throw std::runtime_error("Expected the function buffer to be set, but was nullptr");
        }

        return *this->buffers.function;
    }

    ionshared::Ptr<ionir::BasicBlock> IonIrLoweringPass::requireBasicBlock() {
        if (!ionshared::util::hasValue(this->buffers.basicBlock)) {
            throw std::runtime_error("Expected the basic block buffer to be set, but was nullptr");
        }

        return *this->buffers.basicBlock;
    }

    ionshared::Ptr<ionir::InstBuilder> IonIrLoweringPass::requireBuilder() {
        // Builder must be instantiated.
        if (!this->buffers.builder.has_value()) {
            // Otherwise, throw a runtime error.
            throw std::runtime_error("Expected builder to be instantiated");
        }

        return *buffers.builder;
    }

    void IonIrLoweringPass::setBuilder(ionshared::Ptr<ionir::BasicBlock> basicBlock) {
        this->buffers.builder = basicBlock->createBuilder();
        this->buffers.basicBlock = basicBlock;
    }

    void IonIrLoweringPass::stashBuffers(const std::function<void()>& callback) {
        Buffers buffersBackup = this->buffers;

        callback();
        this->buffers = buffersBackup;
    }

    uint32_t IonIrLoweringPass::getNameCounter() noexcept {
        return this->nameCounter++;
    }

    IonIrLoweringPass::IonIrLoweringPass(
        ionshared::Ptr<ionshared::PassContext> context,
        ionshared::PtrSymbolTable<ionir::Module> modules
    ) :
        Pass(std::move(context)),
        modules(std::move(modules)),
        buffers(),
        symbolTable(),
        nameCounter(0) {
        //
    }

    // TODO
    IonIrLoweringPass::~IonIrLoweringPass() = default;

    ionshared::Ptr<ionshared::SymbolTable<ionshared::Ptr<ionir::Module>>> IonIrLoweringPass::getModules() const {
        return this->modules;
    }

    ionshared::OptPtr<ionir::Module> IonIrLoweringPass::getModuleBuffer() const {
        return this->buffers.module;
    }

    bool IonIrLoweringPass::setModuleBuffer(const std::string& id) {
        if (this->modules->contains(id)) {
            this->buffers.module = this->modules->lookup(id);

            return true;
        }

        return false;
    }

    void IonIrLoweringPass::visit(ionshared::Ptr<Construct> construct) {
        // Prevent construct from being emitted more than once.
        if (this->symbolTable.contains(construct)) {
            return;
        }

        /**
         * Only instruct the node to visit this instance and
         * not its children, since they're already visited by
         * the other member methods.
         */
        construct->accept(*this);
    }

    void IonIrLoweringPass::visitModule(ionshared::Ptr<Module> construct) {
        this->buffers.module = std::make_shared<ionir::Module>(
            std::make_shared<ionir::Identifier>(construct->name)
        );

        // Set the module on the modules symbol table.
        this->modules->set(construct->name, *this->buffers.module);

        // Proceed to visit all the module's children (top-level constructs).
        std::map<std::string, ionshared::Ptr<Construct>> moduleSymbolTable =
            construct->context->getGlobalScope()->unwrap();

        for (const auto &[id, topLevelConstruct] : moduleSymbolTable) {
            this->visit(topLevelConstruct);
        }
    }

    void IonIrLoweringPass::visitFunction(ionshared::Ptr<Function> construct) {
        ionshared::Ptr<ionir::Module> ionIrModuleBuffer = this->requireModule();
        std::string ionIrFunctionId = construct->prototype->name;

        if (ionIrModuleBuffer->context->getGlobalScope()->contains(ionIrFunctionId)) {
            throw ionshared::util::quickError(
                IONLANG_NOTICE_FUNCTION_ALREADY_DEFINED,
                ionIrFunctionId
            );
        }

        ionshared::Ptr<ionir::Prototype> ionIrPrototype =
            this->safeEarlyVisitOrLookup<ionir::Prototype>(construct->prototype);

        /**
         * The function's body will be filled when visiting the body. The
         * visit body function will detect that the block being visited is
         * a function body, and will set the buffered function (this function)'s
         * body with the newly created one.
         */
        ionshared::Ptr<ionir::Function> ionIrFunction =
            std::make_shared<ionir::Function>(ionIrPrototype, nullptr);

        // Set the function buffer. This is required when visiting the function body.
        this->buffers.function = ionIrFunction;

        this->visit(construct->body);

        // TODO: Redundant Repetitive assignment?
        // Set the function buffer.
        this->buffers.function = ionIrFunction;

        // Register the newly created function on the buffer module's symbol table.
        ionIrModuleBuffer->insertFunction(ionIrFunction);
        
        this->symbolTable.set(construct, ionIrFunction);
    }

    void IonIrLoweringPass::visitExtern(ionshared::Ptr<Extern> construct) {
        ionshared::Ptr<ionir::Module> ionirModuleBuffer = this->requireModule();

        ionshared::PtrSymbolTable<ionir::Construct> moduleBufferSymbolTable =
            ionirModuleBuffer->context->getGlobalScope();

        ionshared::Ptr<Prototype> prototype = construct->prototype;

        // TODO: Use verify instead.
        if (prototype == nullptr) {
            throw std::runtime_error("Unexpected external definition's prototype to be null");
        }
        else if (moduleBufferSymbolTable->contains(prototype->name)) {
            throw std::runtime_error("Entity with same id already exists on module");
        }

        ionshared::Ptr<ionir::Prototype> ionIrPrototype =
            this->safeEarlyVisitOrLookup<ionir::Prototype>(prototype);

        ionshared::Ptr<ionir::Extern> ionIrExtern =
            std::make_shared<ionir::Extern>(ionirModuleBuffer, ionIrPrototype);

        /**
         * Register the IonIR extern on the module buffer's symbol table.
         * This will allow the IonIR codegen pass to visit the extern.
         */
        moduleBufferSymbolTable->set(prototype->name, ionIrExtern);

        this->symbolTable.set(construct, ionIrExtern);
    }

    void IonIrLoweringPass::visitPrototype(ionshared::Ptr<Prototype> construct) {
        this->requireModule();

        ionshared::Ptr<ionir::Type> ionIrReturnType =
            this->safeEarlyVisitOrLookup<ionir::Type>(construct->returnType);

        ionshared::Ptr<ionir::Args> ionIrArguments = std::make_shared<ionir::Args>();
        ionshared::Ptr<Args> arguments = construct->args;
        auto nativeArguments = arguments->items->unwrap();

        ionIrArguments->isVariable = arguments->isVariable;

        // TODO: Should Args be a construct, and be visited?
        for (const auto& [id, argument] : nativeArguments) {
            ionIrArguments->items->set(
                argument.second,

                std::make_pair(
                    this->safeEarlyVisitOrLookup<ionir::Type>(argument.first),
                    argument.second
                )
            );
        }

        this->symbolTable.set(construct, std::make_shared<ionir::Prototype>(
            construct->name,
            ionIrArguments,
            ionIrReturnType,
            *this->buffers.module
        ));
    }

    void IonIrLoweringPass::visitBlock(ionshared::Ptr<Block> construct) {
        ionshared::Ptr<ionir::Function> ionIrFunctionBuffer = this->requireFunction();
        ionshared::OptPtr<ionir::FunctionBody> ionIrFunctionBody = std::nullopt;
        ionir::BasicBlockKind ionIrBasicBlockKind = ionir::BasicBlockKind::Internal;

        // TODO: Use a counter or something, along with naming depending on the block's parent (ex. if statement parent => if_0, etc.).
        std::string ionIrBasicBlockId = "tmp_block_" + std::to_string(this->getNameCounter());

        if (construct->isFunctionBody()) {
            // TODO: Make function body and push it onto the stack?
            ionIrFunctionBody = std::make_shared<ionir::FunctionBody>(ionIrFunctionBuffer);

            // Apply the newly created function body to the function.
            ionIrFunctionBuffer->body = *ionIrFunctionBody;

            /**
             * Only make the basic block the entry block if no previous block
             * was designated as entry basic block.
             */
            if (!ionIrFunctionBody->get()->hasEntryBasicBlock()) {
                ionIrBasicBlockKind = ionir::BasicBlockKind::Entry;
                ionIrBasicBlockId = ionir::Const::basicBlockEntryId;
            }
        }

        // TODO: Create the block somehow. It requires 'BasicBlockKind', 'registers' and 'insts'.
        ionshared::Ptr<ionir::BasicBlock> ionIrBasicBlock =
            std::make_shared<ionir::BasicBlock>(ionir::BasicBlockOpts{
                ionIrFunctionBuffer->body,
                ionIrBasicBlockKind,
                ionIrBasicBlockId
            });

        /**
         * Create and assign the block to the builder. This will also
         * set/update the block buffer. This builder may be used by subsequently
         * called visit functions, such as when visiting certain statements which
         * need the builder.
         */
        this->setBuilder(ionIrBasicBlock);

        std::vector<ionshared::Ptr<Statement>> statements = construct->statements;

        for (const auto& statement : statements) {
            // Visit the statement.
            this->visit(statement);

            // TODO: IMPORTANT: DO note that (some?) visitStatements, (ex. visitReturnStatement) already register insts via builder. (They use buffered builder, which is set on code above and bound to the buffered block, set above).
            // TODO: Insts and registers must be popped off and added onto the block.
        }

        if (!ionshared::util::hasValue(ionIrFunctionBody)) {
            this->symbolTable.set(construct, ionIrBasicBlock);
        }
        /**
         * Provided block classifies as a function body. Push the resulting
         * function body onto the stack instead.
         */
        else {
            // Register the IonIR basic block on the function body's symbol table.
            ionIrFunctionBody->get()->getSymbolTable()->set(
                ionIrBasicBlockId,
                ionIrBasicBlock
            );

            // TODO: REVIEW!.
            // No value will be pushed onto the stack.
//            this->constructStack.push(*ionIrFunctionBody);
        }
    }

    void IonIrLoweringPass::visitIntegerLiteral(ionshared::Ptr<IntegerLiteral> construct) {
        PtrResolvable<IntegerType> integerTypeResolvable = construct->type;

        if (!integerTypeResolvable->isResolved()) {
            throw std::runtime_error("Type is unresolved");
        }

        ionshared::Ptr<Type> integerType = *construct->type->getValue();

        if (integerType->typeKind != TypeKind::Integer) {
            throw std::runtime_error("Integer value's type must be integer type");
        }

        ionshared::Ptr<ionir::IntegerType> ionIrIntegerType =
            this->safeEarlyVisitOrLookup<ionir::IntegerType>(integerType);

        this->symbolTable.set(construct, std::make_shared<ionir::IntegerLiteral>(
            ionIrIntegerType,
            construct->value
        ));
    }

    void IonIrLoweringPass::visitCharLiteral(ionshared::Ptr<CharLiteral> construct) {
        this->symbolTable.set(
            construct,
            std::make_shared<ionir::CharLiteral>(construct->value)
        );
    }

    void IonIrLoweringPass::visitStringLiteral(ionshared::Ptr<StringLiteral> construct) {
        this->symbolTable.set(
            construct,
            std::make_shared<ionir::StringLiteral>(construct->value)
        );
    }

    void IonIrLoweringPass::visitBooleanLiteral(ionshared::Ptr<BooleanLiteral> construct) {
        this->symbolTable.set(
            construct,
            std::make_shared<ionir::BooleanLiteral>(construct->value)
        );
    }

    void IonIrLoweringPass::visitGlobal(ionshared::Ptr<Global> construct) {
        // Module buffer will be used, therefore it must be set.
        ionshared::Ptr<ionir::Module> ionIrModuleBuffer = this->requireModule();

        ionshared::Ptr<ionir::Type> type =
            this->safeEarlyVisitOrLookup<ionir::Type>(construct->type);

        ionshared::OptPtr<Expression<>> nodeValue = construct->value;
        ionshared::OptPtr<ionir::Value<>> value = std::nullopt;

        // Assign value if applicable.
        if (ionshared::util::hasValue(nodeValue)) {
            // NOTE: Use static pointer cast when downcasting to ionir::Value<>.
            this->safeEarlyVisitOrLookup<ionir::Value<>>(*nodeValue, false);
        }

        ionshared::Ptr<ionir::Global> ionIrGlobalVariable =
            std::make_shared<ionir::Global>(type, construct->name, value);

        // TODO: Check if exists first?

        /**
         * Register the global variable on the buffered module's symbol table.
         * This will allow it to be visited and emitted to LLVM IR during the
         * IonIR codegen phase.
         */
        ionIrModuleBuffer->context->getGlobalScope()->set(
            ionIrGlobalVariable->name,
            ionIrGlobalVariable
        );

        this->symbolTable.set(construct, ionIrGlobalVariable);
    }

    void IonIrLoweringPass::visitIntegerType(ionshared::Ptr<IntegerType> construct) {
        ionir::IntegerKind ionIrIntegerKind;

        /**
         * Create the corresponding IonIR integer type based off the
         * node's integer kind.
         */
        switch (construct->integerKind) {
            case IntegerKind::Int8: {
                ionIrIntegerKind = ionir::IntegerKind::Int8;

                break;
            }

            case IntegerKind::Int16: {
                ionIrIntegerKind = ionir::IntegerKind::Int16;

                break;
            }

            case IntegerKind::Int32: {
                ionIrIntegerKind = ionir::IntegerKind::Int32;

                break;
            }

            case IntegerKind::Int64: {
                ionIrIntegerKind = ionir::IntegerKind::Int64;

                break;
            }

            case IntegerKind::Int128: {
                ionIrIntegerKind = ionir::IntegerKind::Int128;

                break;
            }

            default: {
                throw std::runtime_error("An unrecognized integer kind was provided");
            }
        }

        this->symbolTable.set(construct, IonIrLoweringPass::processTypeQualifiers(
            std::make_shared<ionir::IntegerType>(ionIrIntegerKind, construct->isSigned),
            construct->qualifiers
        ));
    }

    void IonIrLoweringPass::visitBooleanType(ionshared::Ptr<BooleanType> construct) {
        this->symbolTable.set(construct, IonIrLoweringPass::processTypeQualifiers(
            std::make_shared<ionir::BooleanType>(),
            construct->qualifiers
        ));
    }

    void IonIrLoweringPass::visitVoidType(ionshared::Ptr<VoidType> construct) {
        this->symbolTable.set(construct, IonIrLoweringPass::processTypeQualifiers(
            std::make_shared<ionir::VoidType>(),
            construct->qualifiers
        ));
    }

    void IonIrLoweringPass::visitIfStatement(ionshared::Ptr<IfStatement> construct) {
        ionshared::Ptr<ionir::BasicBlock> ionIrBasicBlockBuffer = this->requireBasicBlock();

        ionshared::Ptr<Block> parentBlock = construct->getUnboxedParent();

        ionshared::Ptr<ionir::Construct> ionIrCondition =
            this->safeEarlyVisitOrLookup(construct->condition);

        // TODO: Should verify if ionIrCondition is either Ref<> or Value<> here? Or in type-check?

        /**
         * Transfer all previously emitted instructions into a new continuation
         * block for the buffered block, then link the buffered block with the
         * new block.
         */
        uint32_t splitOrder = construct->getOrder() + 1;

        // TODO: What if, before splitting, the current basic block buffer already has a terminal inst? But that'd mean that there are more insts after a terminal inst which is an error. Who will catch that? Maybe not really needed to be catched here, instead in typecheck pass or something.

        // TODO: What if successor block is considered function body (split inherits?) then no value is pushed onto stack?
        ionshared::Ptr<Block> successorBlock;

        // TODO: What about the symbol table? It needs to be migrated from the split statements onwards.

        /**
         * Split the block from after this if statement, if there are
         * more statements after this if statement.
         */
        if (parentBlock->statements.size() - 1 >= splitOrder) {
            // TODO: Symbol table not being migrated (split does not do this).
            successorBlock = parentBlock->slice(splitOrder);
        }
        /**
         * There are no more upcoming statements after this if statement.
         * Simply create an empty block, with the same parent.
         */
        else {
            // TODO: Symbol table not being migrated.
            successorBlock = std::make_shared<Block>(parentBlock->getUnboxedParent());
        }

        // TODO: Hotfix to avoid function body (thus overriding bufferFunction's body).
        successorBlock->parent = construct;

        ionshared::Ptr<ionir::BasicBlock> ionIrSuccessorBasicBlock =
            this->safeEarlyVisitOrLookup<ionir::BasicBlock>(successorBlock);

        this->visit(construct->consequentBlock);

        ionshared::Ptr<ionir::BasicBlock> ionIrConsequentBasicBlock =
            this->safeEarlyVisitOrLookup<ionir::BasicBlock>(construct->consequentBlock);

        /**
         * Link the current buffered basic block with the if statement's
         * consequent basic block. Create a new builder, as the previously
         * buffered one might have been replaced when visiting other blocks.
         */
        ionIrBasicBlockBuffer->createBuilder()->createBranch(
            ionIrCondition,
            ionIrConsequentBasicBlock,
            ionIrSuccessorBasicBlock
        );

        /**
         * Link the consequent basic block with the newly created
         * buffered basic block's successor.
         */
        ionIrConsequentBasicBlock->link(ionIrSuccessorBasicBlock);

        if (construct->hasAlternativeBlock()) {
            ionshared::Ptr<ionir::BasicBlock> ionIrAlternativeBlock =
                this->safeEarlyVisitOrLookup<ionir::BasicBlock>(*construct->alternativeBlock);

            /**
             * Link the alternative basic block with the buffered basic
             * block's successor.
             */
            ionIrAlternativeBlock->link(ionIrSuccessorBasicBlock);
        }
    }

    void IonIrLoweringPass::visitReturnStatement(ionshared::Ptr<ReturnStatement> construct) {
        ionshared::Ptr<ionir::InstBuilder> ionIrInstBuilder = this->requireBuilder();
        ionshared::OptPtr<ionir::Value<>> ionIrValue = std::nullopt;

        if (construct->hasValue()) {
            // Use a static pointer cast to cast to ionir::Value<>.
            ionIrValue = this->safeEarlyVisitOrLookup<ionir::Value<>>(
                *construct->value,
                false
            );
        }

        this->symbolTable.set(
            construct,
            ionIrInstBuilder->createReturn(ionIrValue)
        );
    }

    void IonIrLoweringPass::visitAssignmentStatement(ionshared::Ptr<AssignmentStatement> construct) {
        ionshared::Ptr<ionir::InstBuilder> ionIrBuilderBuffer = this->requireBuilder();

        if (!construct->variableDeclStatementRef->isResolved()) {
            // TODO: Better error.
            throw std::runtime_error("Expected variable declaration reference to be resolved");
        }

        ionshared::Ptr<ionir::AllocaInst> ionIrAllocaInst =
            this->safeEarlyVisitOrLookup<ionir::AllocaInst>(
                **construct->variableDeclStatementRef
            );

        this->symbolTable.set(construct, ionIrBuilderBuffer->createStore(
            this->safeEarlyVisitOrLookup<ionir::Value<>>(construct->value),
            ionIrAllocaInst
        ));
    }

    void IonIrLoweringPass::visitVariableDecl(ionshared::Ptr<VariableDeclStatement> construct) {
        ionshared::Ptr<ionir::InstBuilder> ionIrInstBuilder = this->requireBuilder();

        ionshared::Ptr<ionir::Type> ionIrType =
            this->safeEarlyVisitOrLookup<ionir::Type>(construct->type);

        ionshared::Ptr<ionir::AllocaInst> ionIrAllocaInst =
            ionIrInstBuilder->createAlloca(construct->name, ionIrType);

        this->symbolTable.set(construct, ionIrAllocaInst);

        // TODO: Value can be an expression as well. Check that ConstructKind == Value, otherwise handle appropriately.
        ionshared::Ptr<ionir::Value<>> ionIrValue =
            this->safeEarlyVisitOrLookup<ionir::Value<>>(construct->value);

        // TODO: Store inst not being set on symbol table. Should it be, or just AllocaInst?
        ionIrInstBuilder->createStore(
            ionIrValue,
            ionIrAllocaInst
        );
    }

    void IonIrLoweringPass::visitCallExpr(ionshared::Ptr<CallExpr> construct) {
        PtrResolvable<> calleeRef = construct->calleeResolvable;

        if (!calleeRef->isResolved()) {
            throw std::runtime_error("Expected callee function reference to be resolved");
        }

        ionshared::Ptr<Construct> callee = *calleeRef->getValue();
        ConstructKind calleeConstructKind = callee->constructKind;

        // The callee must be either a function or an extern.
        if (calleeConstructKind != ConstructKind::Function && calleeConstructKind != ConstructKind::Extern) {
            // TODO: Use DiagnosticBuilder.
            throw std::runtime_error("Callee '%s' is neither a function nor an extern");
        }

        ionshared::Ptr<ionir::Construct> ionIrCallee =
            this->safeEarlyVisitOrLookup(callee);

        std::vector<ionshared::Ptr<ionir::Construct>> ionIrArgs{};

        for (const auto& arg : construct->args) {
            ionIrArgs.push_back(this->safeEarlyVisitOrLookup(arg));
        }

        /**
         * NOTE: At this point, we know that the basic block buffer is
         * set because the builder buffer is set.
         */
        this->symbolTable.set(construct, this->requireBuilder()->createCall(
            ionIrCallee,
            ionIrArgs
        ));
    }

    void IonIrLoweringPass::visitOperationExpr(ionshared::Ptr<OperationExpr> construct) {
        std::optional<ionir::OperatorKind> ionIrOperatorKindResult =
            util::findIonIrOperatorKind(construct->operation);

        if (!ionIrOperatorKindResult.has_value()) {
            throw std::runtime_error("Unknown intrinsic operator kind");
        }

        ionshared::Ptr<ionir::Value<>> ionIrLeftSideValue =
            this->safeEarlyVisitOrLookup<ionir::Value<>>(construct->leftSideValue);

        ionshared::OptPtr<ionir::Value<>> ionIrRightSideValue = std::nullopt;

        if (ionshared::util::hasValue(construct->rightSideValue)) {
            ionIrRightSideValue =
                this->safeEarlyVisitOrLookup<ionir::Value<>>(*construct->rightSideValue);
        }

        this->symbolTable.set(construct, std::make_shared<ionir::OperationValue>(
            *ionIrOperatorKindResult,
            ionIrLeftSideValue,
            ionIrRightSideValue
        ));
    }

    void IonIrLoweringPass::visitStruct(ionshared::Ptr<Struct> construct) {
        ionshared::Ptr<ionir::Module> ionIrModuleBuffer = this->requireModule();
        ionir::Scope globalSymbolTable = ionIrModuleBuffer->context->getGlobalScope();

        // TODO: Check local symbol table too?
        if (globalSymbolTable->contains(construct->name)) {
            // TODO: Use DiagnosticBuilder.
            throw std::runtime_error("Struct was already previously defined in the module");
        }

        auto fieldsMap = construct->fields->unwrap();

        ionir::Fields ionIrFields =
            ionshared::util::makePtrSymbolTable<ionir::Type>();

        for (const auto& [name, type] : fieldsMap) {
            ionIrFields->set(
                name,
                this->safeEarlyVisitOrLookup<ionir::Type>(type)
            );
        }

        ionshared::Ptr<ionir::Struct> ionIrStruct =
            std::make_shared<ionir::Struct>(construct->name, ionIrFields);

        ionIrModuleBuffer->context->getGlobalScope()->set(
            construct->name,
            ionIrStruct
        );

        this->symbolTable.set(construct, ionIrStruct);
    }

    void IonIrLoweringPass::visitStructDefinition(
        ionshared::Ptr<StructDefinition> construct
    ) {
        if (!construct->declaration->isResolved()) {
            // TODO: Use diagnostics.
            throw std::runtime_error("Declaration must be resolved at this point");
        }

        ionshared::Ptr<ionir::Struct> ionIrStructDeclaration =
            this->safeEarlyVisitOrLookup<ionir::Struct>(**construct->declaration);

        std::vector<ionshared::Ptr<ionir::Value<>>> ionIrValues{};

        for (const auto& value : construct->values) {
            ionIrValues.push_back(
                this->safeEarlyVisitOrLookup<ionir::Value<>>(value, false)
            );
        }

        ionshared::Ptr<ionir::StructDefinition> ionIrStructDefinition =
            std::make_shared<ionir::StructDefinition>(
                ionIrStructDeclaration, ionIrValues
            );

        this->symbolTable.set(construct, ionIrStructDefinition);
    }
}
