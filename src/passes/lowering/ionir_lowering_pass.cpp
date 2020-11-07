#include <ionshared/misc/util.h>
#include <ionir/construct/value/integer_literal.h>
#include <ionir/construct/value/char_literal.h>
#include <ionir/construct/value/string_literal.h>
#include <ionir/construct/value/boolean_literal.h>
#include <ionir/construct/function.h>
#include <ionir/construct/global.h>
#include <ionir/construct/extern.h>
#include <ionir/construct/identifier.h>
#include <ionir/construct/type/struct.h>
#include <ionir/construct/type/boolean_type.h>
#include <ionir/construct/type/void_type.h>
#include <ionir/construct/value/struct_definition.h>
#include <ionir/misc/inst_builder.h>
#include <ionlang/construct/statement/return_statement.h>
#include <ionlang/passes/lowering/ionir_lowering_pass.h>
#include <ionlang/diagnostics/diagnostic.h>
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
        std::shared_ptr<ionir::TypeQualifiers> irTypeQualifiers =
            std::make_shared<ionir::TypeQualifiers>();

        auto typeQualifiersSet = qualifiers->unwrap();

        for (const auto &typeQualifier : typeQualifiersSet) {
            ionir::TypeQualifier irTypeQualifier;

            switch (typeQualifier) {
                case TypeQualifier::Pointer: {
                    irTypeQualifier = ionir::TypeQualifier::Pointer;

                    break;
                }

                case TypeQualifier::Reference: {
                    irTypeQualifier = ionir::TypeQualifier::Reference;

                    break;
                }

                case TypeQualifier::Constant: {
                    irTypeQualifier = ionir::TypeQualifier::Constant;

                    break;
                }

                case TypeQualifier::Mutable: {
                    irTypeQualifier = ionir::TypeQualifier::Mutable;

                    break;
                }

                default: {
                    throw std::runtime_error("Unknown type qualifier");
                }
            }

            irTypeQualifiers->add(irTypeQualifier);
        }

        type->qualifiers = irTypeQualifiers;

        return type;
    }

    void IonIrLoweringPass::stashBuffers(const std::function<void()>& callback) {
        IonIrBuffers buffersBackup = this->irBuffers;

        callback();
        this->irBuffers = buffersBackup;
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
        irBuffers(),
        symbolTable(),
        nameCounter(0) {
        //
    }

    std::shared_ptr<ionshared::SymbolTable<std::shared_ptr<ionir::Module>>> IonIrLoweringPass::getModules() const {
        return this->modules;
    }

    void IonIrLoweringPass::visit(std::shared_ptr<Construct> construct) {
        // Prevent construct from being emitted more than once.
        if (this->symbolTable.contains(construct)) {
            return;
        }
        // TODO: Hotfix for circular dependency within template. This else if branch should be removed once addressed.
        else if (construct->constructKind == ConstructKind::Resolvable) {
            this->visitResolvable(construct->staticCast<Resolvable<>>());

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
        std::shared_ptr<ionir::Module> irModuleBuffer = std::make_shared<ionir::Module>(
            std::make_shared<ionir::Identifier>(construct->name)
        );

        this->irBuffers.modules.push(irModuleBuffer);

        // Set the module on the modules symbol table.
        this->modules->set(construct->name, irModuleBuffer);

        // Proceed to visit all the module's children (top-level constructs).
        std::map<std::string, std::shared_ptr<Construct>> moduleNativeSymbolTable =
            construct->context->globalScope->unwrap();

        for (const auto& [id, topLevelConstruct] : moduleNativeSymbolTable) {
            this->visit(topLevelConstruct);
        }

        this->irBuffers.modules.forcePop();
    }

    void IonIrLoweringPass::visitFunction(std::shared_ptr<Function> construct) {
        std::shared_ptr<ionir::Module> irModuleBuffer =
            this->irBuffers.modules.forceGetTopItem();

        if (irModuleBuffer->context->getGlobalScope()->contains(construct->prototype->name)) {
            this->context->diagnosticBuilder
                ->bootstrap(diagnostic::functionRedefinition)
                ->formatMessage(construct->prototype->name)
                ->finish();

            // TODO
            throw std::runtime_error("Awaiting diagnostics implementation during lowering");
        }

        std::shared_ptr<ionir::Prototype> irPrototype =
            this->safeEarlyVisitOrLookup<ionir::Prototype>(construct->prototype);

        // TODO: Consider using safeVisitOrLookup() to fill in the body after its visited, instead of doing so from the visitBasicBlock() or visitFunctionBody() methods.
        /**
         * The function's body will be filled when visiting the body. The
         * visit body function will detect that the block being visited is
         * a function body, and will set the buffered function (this function)'s
         * body with the newly created one.
         */
        std::shared_ptr<ionir::Function> irFunction = ionir::Function::make(
            irPrototype,
            std::vector<std::shared_ptr<ionir::BasicBlock>>{}
        );

        irFunction->parent = irModuleBuffer;
        this->irBuffers.functions.push(irFunction);
        this->visit(construct->body);
        this->irBuffers.functions.forcePop();
        irModuleBuffer->insertFunction(irFunction);
        this->symbolTable.set(construct, irFunction);
    }

    void IonIrLoweringPass::visitExtern(std::shared_ptr<Extern> construct) {
        std::shared_ptr<ionir::Module> irModuleBuffer =
            this->irBuffers.modules.forceGetTopItem();

        ionshared::PtrSymbolTable<ionir::Construct> moduleBufferSymbolTable =
            irModuleBuffer->context->getGlobalScope();

        std::shared_ptr<Prototype> prototype = construct->prototype;

        // TODO: Use verify instead.
        if (prototype == nullptr) {
            throw std::runtime_error("Unexpected external definition's prototype to be null");
        }
        else if (moduleBufferSymbolTable->contains(prototype->name)) {
            throw std::runtime_error("Entity with same id already exists on module");
        }

        std::shared_ptr<ionir::Prototype> irPrototype =
            this->safeEarlyVisitOrLookup<ionir::Prototype>(prototype);

        std::shared_ptr<ionir::Extern> irExtern =
            ionir::Extern::make(irPrototype);

        irExtern->parent = irModuleBuffer;

        /**
         * Register the IonIR extern on the module buffer's symbol table.
         * This will allow the IonIR codegen pass to visit the extern.
         */
        moduleBufferSymbolTable->set(prototype->name, irExtern);

        this->symbolTable.set(construct, irExtern);
    }

    void IonIrLoweringPass::visitPrototype(std::shared_ptr<Prototype> construct) {
        std::shared_ptr<ionir::Type> irReturnType =
            this->safeEarlyVisitOrLookup<ionir::Type>(construct->returnType);

        std::shared_ptr<ionir::Args> irArguments = std::make_shared<ionir::Args>();

        irArguments->isVariable = construct->argumentList->isVariable;

        auto argumentListNativeMap = construct->argumentList->symbolTable->unwrap();

        // TODO: Should Args be a construct, and be visited?
        for (const auto& [name, type] : argumentListNativeMap) {
            irArguments->items->set(
                name,

                std::make_pair(
                    this->safeEarlyVisitOrLookup<ionir::Type>(**type),
                    name
                )
            );
        }

        std::shared_ptr<ionir::Prototype> irPrototype =
            ionir::Prototype::make(
                construct->name,
                irArguments,
                irReturnType
            );

        irPrototype->parent = this->irBuffers.modules.forceGetTopItem();
        this->symbolTable.set(construct, irPrototype);
    }

    void IonIrLoweringPass::visitBlock(std::shared_ptr<Block> construct) {
        std::shared_ptr<ionir::Function> irFunctionBuffer =
            this->irBuffers.functions.forceGetTopItem();

        // TODO: Use a counter or something, along with naming depending on the block's parent (ex. if statement parent => if_0, etc.).
        std::string irBasicBlockId = "tmp_block_" + std::to_string(this->getNameCounter());

        std::shared_ptr<ionir::BasicBlock> irBasicBlock = ionir::BasicBlock::make();

        irBasicBlock->parent = irFunctionBuffer;
        this->irBuffers.basicBlocks.push(irBasicBlock);

        std::vector<std::shared_ptr<Statement>> statements = construct->statements;

        for (const auto& statement : statements) {
            this->visit(statement);
        }

        this->irBuffers.basicBlocks.forcePop();
        irFunctionBuffer->basicBlocks.push_back(irBasicBlock);
        this->symbolTable.set(construct, irBasicBlock);
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

        std::shared_ptr<ionir::IntegerType> irIntegerType =
            this->safeEarlyVisitOrLookup<ionir::IntegerType>(integerType);

        this->symbolTable.set(construct, ionir::IntegerLiteral::make(
            irIntegerType,
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
        std::shared_ptr<ionir::Type> irType =
            this->safeEarlyVisitOrLookup<ionir::Type>(construct->type);

        ionshared::OptPtr<ionir::Value<>> irValue = std::nullopt;

        // Assign value if applicable.
        if (ionshared::util::hasValue(construct->value)) {
            // NOTE: Use static pointer cast when downcasting to ionir::Value<>.
            irValue = this->safeEarlyVisitOrLookup<ionir::Value<>>(
                *construct->value,
                false
            );
        }

        std::shared_ptr<ionir::Global> irGlobal =
            ionir::Global::make(irType, construct->name, irValue);

        // TODO: Check if exists first?

        /**
         * Register the global variable on the buffered module's symbol table.
         * This will allow it to be visited and emitted to LLVM IR during the
         * IonIR codegen phase.
         */
        this->irBuffers.modules.forceGetTopItem()->context->getGlobalScope()->set(
            irGlobal->name,
            irGlobal
        );

        this->symbolTable.set(construct, irGlobal);
    }

    void IonIrLoweringPass::visitIntegerType(std::shared_ptr<IntegerType> construct) {
        ionir::IntegerKind irIntegerKind;

        /**
         * Create the corresponding IonIR integer type based off the
         * node's integer kind.
         */
        switch (construct->integerKind) {
            case IntegerKind::Int8: {
                irIntegerKind = ionir::IntegerKind::Int8;

                break;
            }

            case IntegerKind::Int16: {
                irIntegerKind = ionir::IntegerKind::Int16;

                break;
            }

            case IntegerKind::Int32: {
                irIntegerKind = ionir::IntegerKind::Int32;

                break;
            }

            case IntegerKind::Int64: {
                irIntegerKind = ionir::IntegerKind::Int64;

                break;
            }

            case IntegerKind::Int128: {
                irIntegerKind = ionir::IntegerKind::Int128;

                break;
            }

            default: {
                throw std::runtime_error("An unrecognized integer kind was provided");
            }
        }

        this->symbolTable.set(construct, IonIrLoweringPass::processTypeQualifiers(
            std::make_shared<ionir::IntegerType>(irIntegerKind, construct->isSigned),
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

    void IonIrLoweringPass::visitStructType(std::shared_ptr<StructType> construct) {
        // TODO: What if it's visited as a inline type and not a struct declaration?

        std::shared_ptr<ionir::Module> irModuleBuffer =
            this->irBuffers.modules.forceGetTopItem();

        ionir::Scope globalSymbolTable = irModuleBuffer->context->getGlobalScope();

        // TODO: Check local symbol table too?
        if (globalSymbolTable->contains(construct->typeName)) {
            // TODO: Use DiagnosticBuilder.
            throw std::runtime_error("Struct was already previously defined in the module");
        }

        auto fieldsNativeMap = construct->fields->unwrap();

        ionir::Fields irFields =
            ionshared::util::makePtrSymbolTable<ionir::Type>();

        for (const auto& [name, irType] : fieldsNativeMap) {
            irFields->set(
                name,
                this->safeEarlyVisitOrLookup<ionir::Type>(irType)
            );
        }

        std::shared_ptr<ionir::StructType> irStruct =
            ionir::StructType::make(construct->typeName, irFields);

        irStruct->parent = this->irBuffers.modules.forceGetTopItem();

        irModuleBuffer->context->getGlobalScope()->set(
            construct->typeName,
            irStruct
        );

        this->symbolTable.set(construct, irStruct);
    }

    void IonIrLoweringPass::visitIfStmt(std::shared_ptr<IfStmt> construct) {
        std::shared_ptr<Block> parentBlock = construct->forceGetUnboxedParent();

        std::shared_ptr<ionir::Construct> irCondition =
            this->safeEarlyVisitOrLookup(construct->condition);

        // TODO: Should verify if irCondition is either Ref<> or Value<> here? Or in type-check?

        /**
         * Transfer all previously emitted instructions into a new continuation
         * block for the buffered block, then link the buffered block with the
         * new block.
         */
        uint32_t splitOrder = construct->getOrder() + 1;

        // TODO: What if, before splitting, the current basic block buffer already has a terminal inst? But that'd mean that there are more instructions after a terminal inst which is an error. Who will catch that? Maybe not really needed to be caught here, instead in type-check pass or something.

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
            successorBlock = Block::make();
            successorBlock->parent = parentBlock->parent;
        }

        // TODO: Hotfix to avoid function body (thus overriding bufferFunction's body).
        successorBlock->parent = construct;

        std::shared_ptr<ionir::BasicBlock> irSuccessorBasicBlock =
            this->safeEarlyVisitOrLookup<ionir::BasicBlock>(successorBlock);

        this->visit(construct->consequentBlock);

        std::shared_ptr<ionir::BasicBlock> irConsequentBasicBlock =
            this->safeEarlyVisitOrLookup<ionir::BasicBlock>(construct->consequentBlock);

        /**
         * Link the current buffered basic block with the if statement's
         * consequent basic block. Create a new builder, as the previously
         * buffered one might have been replaced when visiting other blocks.
         */
        this->irBuffers.makeBuilder()->createBranch(
            irCondition,
            irConsequentBasicBlock,
            irSuccessorBasicBlock
        );

        /**
         * Link the consequent basic block with the newly created
         * buffered basic block's successor.
         */
        irConsequentBasicBlock->link(irSuccessorBasicBlock);

        if (construct->hasAlternativeBlock()) {
            std::shared_ptr<ionir::BasicBlock> irAlternativeBlock =
                this->safeEarlyVisitOrLookup<ionir::BasicBlock>(*construct->alternativeBlock);

            /**
             * Link the alternative basic block with the buffered basic
             * block's successor.
             */
            irAlternativeBlock->link(irSuccessorBasicBlock);
        }
    }

    void IonIrLoweringPass::visitReturnStmt(std::shared_ptr<ReturnStmt> construct) {
        ionshared::OptPtr<ionir::Value<>> irValue = std::nullopt;

        if (construct->hasValue()) {
            // Use a static pointer cast to cast to ionir::Value<>.
            irValue = this->safeEarlyVisitOrLookup<ionir::Value<>>(
                *construct->value,
                false
            );
        }

        this->symbolTable.set(
            construct,
            this->irBuffers.makeBuilder()->createReturn(irValue)
        );
    }

    void IonIrLoweringPass::visitAssignmentStmt(std::shared_ptr<AssignmentStmt> construct) {
        if (!construct->variableDeclStmtRef->isResolved()) {
            // TODO: Better error.
            throw std::runtime_error("Expected variable declaration reference to be resolved");
        }

        std::shared_ptr<ionir::AllocaInst> irAllocaInst =
            this->safeEarlyVisitOrLookup<ionir::AllocaInst>(
                **construct->variableDeclStmtRef
            );

        this->symbolTable.set(
            construct,

            this->irBuffers.makeBuilder()->createStore(
                this->safeEarlyVisitOrLookup<ionir::Value<>>(construct->value, false),
                irAllocaInst
            )
        );
    }

    void IonIrLoweringPass::visitVariableDeclStmt(std::shared_ptr<VariableDeclStmt> construct) {
        std::shared_ptr<ionir::InstBuilder> irInstBuilder =
            this->irBuffers.makeBuilder();

        std::shared_ptr<ionir::Type> irType =
            this->safeEarlyVisitOrLookup<ionir::Type>(construct->type);

        std::shared_ptr<ionir::AllocaInst> irAllocaInst =
            irInstBuilder->createAlloca(construct->name, irType);

        this->symbolTable.set(construct, irAllocaInst);

        // TODO: Value can be an expression as well. Check that ConstructKind == Value, otherwise handle appropriately.
        std::shared_ptr<ionir::Value<>> irValue =
            this->safeEarlyVisitOrLookup<ionir::Value<>>(construct->value, false);

        // TODO: Store inst not being set on symbol table. Should it be, or just AllocaInst?
        irInstBuilder->createStore(
            irValue,
            irAllocaInst
        );
    }

    void IonIrLoweringPass::visitExprWrapperStmt(std::shared_ptr<ExprWrapperStmt> construct) {
        this->symbolTable.set(
            construct,
            this->safeEarlyVisitOrLookup(construct->expression)
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

        std::shared_ptr<ionir::Construct> irCallee =
            this->safeEarlyVisitOrLookup(callee);

        std::vector<std::shared_ptr<ionir::Construct>> irArgs{};

        for (const auto& arg : construct->arguments) {
            irArgs.push_back(this->safeEarlyVisitOrLookup(arg));
        }

        // TODO: Outdated doc.
        /**
         * NOTE: At this point, we know that the basic block buffer is
         * set because the builder buffer is set.
         */
        this->symbolTable.set(
            construct,

            this->irBuffers.makeBuilder()->createCall(
                irCallee,
                irArgs
            )
        );
    }

    void IonIrLoweringPass::visitOperationExpr(std::shared_ptr<OperationExpr> construct) {
        std::optional<ionir::OperatorKind> irOperatorKindResult =
            util::findIonIrOperatorKind(construct->operation);

        if (!irOperatorKindResult.has_value()) {
            throw std::runtime_error("Unknown intrinsic operator kind");
        }

        std::shared_ptr<ionir::Value<>> irLeftSideValue =
            this->safeEarlyVisitOrLookup<ionir::Value<>>(construct->leftSideValue, false);

        ionshared::OptPtr<ionir::Value<>> irRightSideValue = std::nullopt;

        if (ionshared::util::hasValue(construct->rightSideValue)) {
            irRightSideValue =
                this->safeEarlyVisitOrLookup<ionir::Value<>>(*construct->rightSideValue, false);
        }

        this->symbolTable.set(construct, ionir::OperationValue::make(
            *irOperatorKindResult,
            irLeftSideValue,
            irRightSideValue
        ));
    }

    void IonIrLoweringPass::visitVariableRefExpr(std::shared_ptr<VariableRefExpr> construct) {
        this->symbolTable.set(
            construct,

            this->safeEarlyVisitOrLookup<ionir::AllocaInst>(
                **construct->variableDecl
            )
        );
    }

    void IonIrLoweringPass::visitStructDefinition(
        std::shared_ptr<StructDefinition> construct
    ) {
        std::shared_ptr<ionir::StructType> irStructType =
            this->safeEarlyVisitOrLookup<ionir::StructType>(**construct->type);

        std::vector<std::shared_ptr<ionir::Value<>>> irValues{};

        for (const auto& value : construct->values) {
            irValues.push_back(
                this->safeEarlyVisitOrLookup<ionir::Value<>>(value, false)
            );
        }

        this->symbolTable.set(construct, ionir::StructDefinition::make(
            irStructType,
            irValues
        ));
    }

    void IonIrLoweringPass::visitResolvable(PtrResolvable<> construct) {
        if (!construct->isResolved()) {
            // TODO: Use diagnostics API (internal error).
            throw std::runtime_error("Expected resolvable to be resolved at this point");
        }

        this->symbolTable.set(
            construct,
            this->safeEarlyVisitOrLookup(*construct->getValue())
        );
    }

    void IonIrLoweringPass::visitCast(std::shared_ptr<Cast> construct) {
        // TODO
    }
}
