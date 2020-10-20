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
    std::shared_ptr<ionir::InstBuilder> IonIrLoweringPass::IonIrBuffers::makeBuilder() {
        return this->basicBlocks.forceGetTopItem()->createBuilder();
    }

    std::shared_ptr<ionir::Type> IonIrLoweringPass::processTypeQualifiers(
        std::shared_ptr<ionir::Type> type,
        const std::shared_ptr<TypeQualifiers>& qualifiers
    ) {
        std::shared_ptr<ionir::TypeQualifiers> ionIrTypeQualifiers =
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

    void IonIrLoweringPass::stashBuffers(const std::function<void()>& callback) {
        IonIrBuffers buffersBackup = this->ionIrBuffers;

        callback();
        this->ionIrBuffers = buffersBackup;
    }

    uint32_t IonIrLoweringPass::getNameCounter() noexcept {
        return this->nameCounter++;
    }

    IonIrLoweringPass::IonIrLoweringPass(
        std::shared_ptr<ionshared::PassContext> context,
        ionshared::PtrSymbolTable<ionir::Module> modules
    ) :
        Pass(std::move(context)),
        modules(std::move(modules)),
        ionIrBuffers(),
        symbolTable(),
        nameCounter(0) {
        //
    }

    // TODO
    IonIrLoweringPass::~IonIrLoweringPass() = default;

    std::shared_ptr<ionshared::SymbolTable<std::shared_ptr<ionir::Module>>> IonIrLoweringPass::getModules() const {
        return this->modules;
    }

    void IonIrLoweringPass::visit(std::shared_ptr<Construct> construct) {
        // Prevent construct from being emitted more than once.
        if (this->symbolTable.contains(construct)) {
            return;
        }

        /**
         * Only instruct the construct to visit this instance and
         * not its children, since they're already visited by
         * the other member methods.
         */
        construct->accept(*this);
    }

    void IonIrLoweringPass::visitModule(std::shared_ptr<Module> construct) {
        std::shared_ptr<ionir::Module> ionIrModuleBuffer = std::make_shared<ionir::Module>(
            std::make_shared<ionir::Identifier>(construct->name)
        );

        this->ionIrBuffers.modules.push(ionIrModuleBuffer);

        // Set the module on the modules symbol table.
        this->modules->set(construct->name, ionIrModuleBuffer);

        // Proceed to visit all the module's children (top-level constructs).
        std::map<std::string, std::shared_ptr<Construct>> moduleNativeSymbolTable =
            construct->context->getGlobalScope()->unwrap();

        for (const auto& [id, topLevelConstruct] : moduleNativeSymbolTable) {
            this->visit(topLevelConstruct);
        }

        this->ionIrBuffers.modules.forcePop();
    }

    void IonIrLoweringPass::visitFunction(std::shared_ptr<Function> construct) {
        std::shared_ptr<ionir::Module> ionIrModuleBuffer =
            this->ionIrBuffers.modules.forceGetTopItem();

        std::string ionIrFunctionId = construct->prototype->name;

        if (ionIrModuleBuffer->context->getGlobalScope()->contains(ionIrFunctionId)) {
            // TODO: Use diagnostics API.
            throw ionshared::util::quickError(
                IONLANG_NOTICE_FUNCTION_ALREADY_DEFINED,
                ionIrFunctionId
            );
        }

        std::shared_ptr<ionir::Prototype> ionIrPrototype =
            this->safeEarlyVisitOrLookup<ionir::Prototype>(construct->prototype);

        // TODO: Consider using safeVisitOrLookup() to fill in the body after its visited, instead of doing so from the visitBasicBlock() or visitFunctionBody() methods.
        /**
         * The function's body will be filled when visiting the body. The
         * visit body function will detect that the block being visited is
         * a function body, and will set the buffered function (this function)'s
         * body with the newly created one.
         */
        std::shared_ptr<ionir::Function> ionIrFunction =
            std::make_shared<ionir::Function>(ionIrPrototype, nullptr);

        this->ionIrBuffers.functions.push(ionIrFunction);
        this->visit(construct->body);
        this->ionIrBuffers.functions.forcePop();
        ionIrModuleBuffer->insertFunction(ionIrFunction);
        this->symbolTable.set(construct, ionIrFunction);
    }

    void IonIrLoweringPass::visitExtern(std::shared_ptr<Extern> construct) {
        std::shared_ptr<ionir::Module> ionirModuleBuffer =
            this->ionIrBuffers.modules.forceGetTopItem();

        ionshared::PtrSymbolTable<ionir::Construct> moduleBufferSymbolTable =
            ionirModuleBuffer->context->getGlobalScope();

        std::shared_ptr<Prototype> prototype = construct->prototype;

        // TODO: Use verify instead.
        if (prototype == nullptr) {
            throw std::runtime_error("Unexpected external definition's prototype to be null");
        }
        else if (moduleBufferSymbolTable->contains(prototype->name)) {
            throw std::runtime_error("Entity with same id already exists on module");
        }

        std::shared_ptr<ionir::Prototype> ionIrPrototype =
            this->safeEarlyVisitOrLookup<ionir::Prototype>(prototype);

        std::shared_ptr<ionir::Extern> ionIrExtern =
            std::make_shared<ionir::Extern>(ionirModuleBuffer, ionIrPrototype);

        /**
         * Register the IonIR extern on the module buffer's symbol table.
         * This will allow the IonIR codegen pass to visit the extern.
         */
        moduleBufferSymbolTable->set(prototype->name, ionIrExtern);

        this->symbolTable.set(construct, ionIrExtern);
    }

    void IonIrLoweringPass::visitPrototype(std::shared_ptr<Prototype> construct) {
        std::shared_ptr<ionir::Type> ionIrReturnType =
            this->safeEarlyVisitOrLookup<ionir::Type>(construct->returnType);

        std::shared_ptr<ionir::Args> ionIrArguments = std::make_shared<ionir::Args>();
        std::shared_ptr<Args> arguments = construct->args;
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
            this->ionIrBuffers.modules.forceGetTopItem()
        ));
    }

    void IonIrLoweringPass::visitBlock(std::shared_ptr<Block> construct) {
        std::shared_ptr<ionir::Function> ionIrFunctionBuffer =
            this->ionIrBuffers.functions.forceGetTopItem();

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
        std::shared_ptr<ionir::BasicBlock> ionIrBasicBlock =
            std::make_shared<ionir::BasicBlock>(ionir::BasicBlockOpts{
                ionIrFunctionBuffer->body,
                ionIrBasicBlockKind,
                ionIrBasicBlockId
            });

        this->ionIrBuffers.basicBlocks.push(ionIrBasicBlock);

        std::vector<std::shared_ptr<Statement>> statements = construct->statements;

        for (const auto& statement : statements) {
            // Visit the statement.
            this->visit(statement);

            // TODO: IMPORTANT: DO note that (some?) visitStatements, (ex. visitReturnStatement) already register insts via builder. (They use buffered builder, which is set on code above and bound to the buffered block, set above).
            // TODO: Insts and registers must be popped off and added onto the block.
        }

        this->ionIrBuffers.basicBlocks.forcePop();

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

    void IonIrLoweringPass::visitIntegerLiteral(std::shared_ptr<IntegerLiteral> construct) {
        PtrResolvable<IntegerType> integerTypeResolvable = construct->type;

        if (!integerTypeResolvable->isResolved()) {
            throw std::runtime_error("Type is unresolved");
        }

        std::shared_ptr<Type> integerType = *construct->type->getValue();

        if (integerType->typeKind != TypeKind::Integer) {
            throw std::runtime_error("Integer value's type must be integer type");
        }

        std::shared_ptr<ionir::IntegerType> ionIrIntegerType =
            this->safeEarlyVisitOrLookup<ionir::IntegerType>(integerType);

        this->symbolTable.set(construct, std::make_shared<ionir::IntegerLiteral>(
            ionIrIntegerType,
            construct->value
        ));
    }

    void IonIrLoweringPass::visitCharLiteral(std::shared_ptr<CharLiteral> construct) {
        this->symbolTable.set(
            construct,
            std::make_shared<ionir::CharLiteral>(construct->value)
        );
    }

    void IonIrLoweringPass::visitStringLiteral(std::shared_ptr<StringLiteral> construct) {
        this->symbolTable.set(
            construct,
            std::make_shared<ionir::StringLiteral>(construct->value)
        );
    }

    void IonIrLoweringPass::visitBooleanLiteral(std::shared_ptr<BooleanLiteral> construct) {
        this->symbolTable.set(
            construct,
            std::make_shared<ionir::BooleanLiteral>(construct->value)
        );
    }

    void IonIrLoweringPass::visitGlobal(std::shared_ptr<Global> construct) {
        std::shared_ptr<ionir::Type> type =
            this->safeEarlyVisitOrLookup<ionir::Type>(construct->type);

        ionshared::OptPtr<Expression<>> nodeValue = construct->value;
        ionshared::OptPtr<ionir::Value<>> value = std::nullopt;

        // Assign value if applicable.
        if (ionshared::util::hasValue(nodeValue)) {
            // NOTE: Use static pointer cast when downcasting to ionir::Value<>.
            this->safeEarlyVisitOrLookup<ionir::Value<>>(*nodeValue, false);
        }

        std::shared_ptr<ionir::Global> ionIrGlobalVariable =
            std::make_shared<ionir::Global>(type, construct->name, value);

        // TODO: Check if exists first?

        /**
         * Register the global variable on the buffered module's symbol table.
         * This will allow it to be visited and emitted to LLVM IR during the
         * IonIR codegen phase.
         */
        this->ionIrBuffers.modules.forceGetTopItem()->context->getGlobalScope()->set(
            ionIrGlobalVariable->name,
            ionIrGlobalVariable
        );

        this->symbolTable.set(construct, ionIrGlobalVariable);
    }

    void IonIrLoweringPass::visitIntegerType(std::shared_ptr<IntegerType> construct) {
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

    void IonIrLoweringPass::visitBooleanType(std::shared_ptr<BooleanType> construct) {
        this->symbolTable.set(construct, IonIrLoweringPass::processTypeQualifiers(
            std::make_shared<ionir::BooleanType>(),
            construct->qualifiers
        ));
    }

    void IonIrLoweringPass::visitVoidType(std::shared_ptr<VoidType> construct) {
        this->symbolTable.set(construct, IonIrLoweringPass::processTypeQualifiers(
            std::make_shared<ionir::VoidType>(),
            construct->qualifiers
        ));
    }

    void IonIrLoweringPass::visitIfStatement(std::shared_ptr<IfStatement> construct) {
        std::shared_ptr<Block> parentBlock = construct->getUnboxedParent();

        std::shared_ptr<ionir::Construct> ionIrCondition =
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
        std::shared_ptr<Block> successorBlock;

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

        std::shared_ptr<ionir::BasicBlock> ionIrSuccessorBasicBlock =
            this->safeEarlyVisitOrLookup<ionir::BasicBlock>(successorBlock);

        this->visit(construct->consequentBlock);

        std::shared_ptr<ionir::BasicBlock> ionIrConsequentBasicBlock =
            this->safeEarlyVisitOrLookup<ionir::BasicBlock>(construct->consequentBlock);

        /**
         * Link the current buffered basic block with the if statement's
         * consequent basic block. Create a new builder, as the previously
         * buffered one might have been replaced when visiting other blocks.
         */
        this->ionIrBuffers.makeBuilder()->createBranch(
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
            std::shared_ptr<ionir::BasicBlock> ionIrAlternativeBlock =
                this->safeEarlyVisitOrLookup<ionir::BasicBlock>(*construct->alternativeBlock);

            /**
             * Link the alternative basic block with the buffered basic
             * block's successor.
             */
            ionIrAlternativeBlock->link(ionIrSuccessorBasicBlock);
        }
    }

    void IonIrLoweringPass::visitReturnStatement(std::shared_ptr<ReturnStatement> construct) {
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
            this->ionIrBuffers.makeBuilder()->createReturn(ionIrValue)
        );
    }

    void IonIrLoweringPass::visitAssignmentStatement(std::shared_ptr<AssignmentStatement> construct) {
        if (!construct->variableDeclStatementRef->isResolved()) {
            // TODO: Better error.
            throw std::runtime_error("Expected variable declaration reference to be resolved");
        }

        std::shared_ptr<ionir::AllocaInst> ionIrAllocaInst =
            this->safeEarlyVisitOrLookup<ionir::AllocaInst>(
                **construct->variableDeclStatementRef
            );

        this->symbolTable.set(
            construct,

            this->ionIrBuffers.makeBuilder()->createStore(
                this->safeEarlyVisitOrLookup<ionir::Value<>>(construct->value, false),
                ionIrAllocaInst
            )
        );
    }

    void IonIrLoweringPass::visitVariableDecl(std::shared_ptr<VariableDeclStatement> construct) {
        std::shared_ptr<ionir::InstBuilder> ionIrInstBuilder =
            this->ionIrBuffers.makeBuilder();

        std::shared_ptr<ionir::Type> ionIrType =
            this->safeEarlyVisitOrLookup<ionir::Type>(construct->type);

        std::shared_ptr<ionir::AllocaInst> ionIrAllocaInst =
            ionIrInstBuilder->createAlloca(construct->name, ionIrType);

        this->symbolTable.set(construct, ionIrAllocaInst);

        // TODO: Value can be an expression as well. Check that ConstructKind == Value, otherwise handle appropriately.
        std::shared_ptr<ionir::Value<>> ionIrValue =
            this->safeEarlyVisitOrLookup<ionir::Value<>>(construct->value, false);

        // TODO: Store inst not being set on symbol table. Should it be, or just AllocaInst?
        ionIrInstBuilder->createStore(
            ionIrValue,
            ionIrAllocaInst
        );
    }

    void IonIrLoweringPass::visitCallExpr(std::shared_ptr<CallExpr> construct) {
        PtrResolvable<> calleeRef = construct->calleeResolvable;

        if (!calleeRef->isResolved()) {
            throw std::runtime_error("Expected callee function reference to be resolved");
        }

        std::shared_ptr<Construct> callee = *calleeRef->getValue();
        ConstructKind calleeConstructKind = callee->constructKind;

        // The callee must be either a function or an extern.
        if (calleeConstructKind != ConstructKind::Function && calleeConstructKind != ConstructKind::Extern) {
            // TODO: Use DiagnosticBuilder.
            throw std::runtime_error("Callee '%s' is neither a function nor an extern");
        }

        std::shared_ptr<ionir::Construct> ionIrCallee =
            this->safeEarlyVisitOrLookup(callee);

        std::vector<std::shared_ptr<ionir::Construct>> ionIrArgs{};

        for (const auto& arg : construct->args) {
            ionIrArgs.push_back(this->safeEarlyVisitOrLookup(arg));
        }

        /**
         * NOTE: At this point, we know that the basic block buffer is
         * set because the builder buffer is set.
         */
        this->symbolTable.set(construct, this->ionIrBuffers.makeBuilder()->createCall(
            ionIrCallee,
            ionIrArgs
        ));
    }

    void IonIrLoweringPass::visitOperationExpr(std::shared_ptr<OperationExpr> construct) {
        std::optional<ionir::OperatorKind> ionIrOperatorKindResult =
            util::findIonIrOperatorKind(construct->operation);

        if (!ionIrOperatorKindResult.has_value()) {
            throw std::runtime_error("Unknown intrinsic operator kind");
        }

        std::shared_ptr<ionir::Value<>> ionIrLeftSideValue =
            this->safeEarlyVisitOrLookup<ionir::Value<>>(construct->leftSideValue, false);

        ionshared::OptPtr<ionir::Value<>> ionIrRightSideValue = std::nullopt;

        if (ionshared::util::hasValue(construct->rightSideValue)) {
            ionIrRightSideValue =
                this->safeEarlyVisitOrLookup<ionir::Value<>>(*construct->rightSideValue, false);
        }

        this->symbolTable.set(construct, std::make_shared<ionir::OperationValue>(
            *ionIrOperatorKindResult,
            ionIrLeftSideValue,
            ionIrRightSideValue
        ));
    }

    void IonIrLoweringPass::visitStruct(std::shared_ptr<Struct> construct) {
        std::shared_ptr<ionir::Module> ionIrModuleBuffer =
            this->ionIrBuffers.modules.forceGetTopItem();

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

        std::shared_ptr<ionir::Struct> ionIrStruct =
            std::make_shared<ionir::Struct>(construct->name, ionIrFields);

        ionIrModuleBuffer->context->getGlobalScope()->set(
            construct->name,
            ionIrStruct
        );

        this->symbolTable.set(construct, ionIrStruct);
    }

    void IonIrLoweringPass::visitStructDefinition(
        std::shared_ptr<StructDefinition> construct
    ) {
        if (!construct->declaration->isResolved()) {
            // TODO: Use diagnostics.
            throw std::runtime_error("Declaration must be resolved at this point");
        }

        std::shared_ptr<ionir::Struct> ionIrStructDeclaration =
            this->safeEarlyVisitOrLookup<ionir::Struct>(**construct->declaration);

        std::vector<std::shared_ptr<ionir::Value<>>> ionIrValues{};

        for (const auto& value : construct->values) {
            ionIrValues.push_back(
                this->safeEarlyVisitOrLookup<ionir::Value<>>(value, false)
            );
        }

        std::shared_ptr<ionir::StructDefinition> ionIrStructDefinition =
            std::make_shared<ionir::StructDefinition>(
                ionIrStructDeclaration, ionIrValues
            );

        this->symbolTable.set(construct, ionIrStructDefinition);
    }
}
