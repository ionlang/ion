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

    void IonIrLoweringPass::lockBuffers(const std::function<void()>& callback) {
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
        constructStack(),
        typeStack(),
        buffers(),
        symbolTable(),
        nameCounter(0) {
        //
    }

    // TODO
    IonIrLoweringPass::~IonIrLoweringPass() = default;

    ionshared::Stack<ionshared::Ptr<ionir::Construct>> IonIrLoweringPass::getConstructStack() const noexcept {
        return this->constructStack;
    }

    ionshared::Stack<ionshared::Ptr<ionir::Type>> IonIrLoweringPass::getTypeStack() const noexcept {
        return this->typeStack;
    }

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

    void IonIrLoweringPass::visit(ionshared::Ptr<Construct> node) {
        /**
         * Only instruct the node to visit this instance and
         * not its children, since they're already visited by
         * the other member methods.
         */
        node->accept(*this);
    }

    void IonIrLoweringPass::visitModule(ionshared::Ptr<Module> node) {
        this->buffers.module = std::make_shared<ionir::Module>(
            std::make_shared<ionir::Identifier>(node->name)
        );

        // Set the module on the modules symbol table.
        this->modules->set(node->name, *this->buffers.module);

        // Proceed to visit all the module's children (top-level constructs).
        std::map<std::string, ionshared::Ptr<Construct>> moduleSymbolTable =
            node->context->getGlobalScope()->unwrap();

        for (const auto &[id, topLevelConstruct] : moduleSymbolTable) {
            this->visit(topLevelConstruct);

            /**
             * Discard visited top-level constructs (such as functions and
             * global variables) as they have no use elsewhere.
             */
            this->constructStack.tryPop();
        }
    }

    void IonIrLoweringPass::visitFunction(ionshared::Ptr<Function> node) {
        if (this->symbolTable.contains(node)) {
            return;
        }

        ionshared::Ptr<ionir::Module> moduleBuffer = this->requireModule();

        // TODO: Awaiting verification implementation.
//        if (!node->verify()) {
//            throw ionshared::util::quickError(
//                IONLANG_NOTICE_MISC_VERIFICATION_FAILED,
//                "Function" // TODO: Hard-coded, should be using Util::getConstructName().
//            );
//        }

        std::string ionIrFunctionId = node->prototype->name;

        if (moduleBuffer->context->getGlobalScope()->contains(ionIrFunctionId)) {
            throw ionshared::util::quickError(
                IONLANG_NOTICE_FUNCTION_ALREADY_DEFINED,
                ionIrFunctionId
            );
        }

        this->visitPrototype(node->prototype);

        ionshared::Ptr<ionir::Prototype> ionIrPrototype =
            this->constructStack.pop()->dynamicCast<ionir::Prototype>();

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

        this->visitBlock(node->body);

        // TODO: Redundant Repetitive assignment?
        // Set the function buffer.
        this->buffers.function = ionIrFunction;

        // Register the newly created function on the buffer module's symbol table.
        moduleBuffer->insertFunction(ionIrFunction);

        /**
         * Register the node and function on the local symbol table for later lookups,
         * ex. when visiting a call expression.
         */
        this->symbolTable.set(node, ionIrFunction);

        // Push the function back onto the stack.
        this->constructStack.push(ionIrFunction);
    }

    void IonIrLoweringPass::visitExtern(ionshared::Ptr<Extern> node) {
        if (this->symbolTable.contains(node)) {
            return;
        }

        ionshared::Ptr<ionir::Module> ionirModuleBuffer = this->requireModule();

        ionshared::PtrSymbolTable<ionir::Construct> moduleBufferSymbolTable =
            ionirModuleBuffer->context->getGlobalScope();

        ionshared::Ptr<Prototype> prototype = node->prototype;

        // TODO: Use verify instead.
        if (prototype == nullptr) {
            throw std::runtime_error("Unexpected external definition's prototype to be null");
        }

        if (moduleBufferSymbolTable->contains(prototype->name)) {
            throw std::runtime_error("Entity with same id already exists on module");
        }

        this->visitPrototype(prototype);

        ionshared::Ptr<ionir::Prototype> ionIrPrototype =
            this->constructStack.pop()->dynamicCast<ionir::Prototype>();

        ionshared::Ptr<ionir::Extern> ionIrExtern =
            std::make_shared<ionir::Extern>(ionirModuleBuffer, ionIrPrototype);

        /**
         * Register the IonIR extern on the module buffer's symbol table.
         * This will allow the IonIR codegen pass to visit the extern.
         */
        moduleBufferSymbolTable->set(prototype->name, ionIrExtern);

        this->symbolTable.set(node, ionIrExtern);
        this->constructStack.push(ionIrExtern);
    }

    void IonIrLoweringPass::visitPrototype(ionshared::Ptr<Prototype> node) {
        this->requireModule();
        this->visit(node->returnType);

        ionshared::Ptr<ionir::Type> ionIrReturnType = this->typeStack.pop();
        ionshared::Ptr<ionir::Args> ionIrArguments = std::make_shared<ionir::Args>();
        ionshared::Ptr<Args> arguments = node->args;
        auto nativeArguments = arguments->items->unwrap();

        ionIrArguments->isVariable = arguments->isVariable;

        // TODO: Should Args be a construct, and be visited?
        for (const auto &[id, argument] : nativeArguments) {
            this->visit(argument.first);

            ionIrArguments->items->set(
                argument.second,
                std::make_pair(this->typeStack.pop(), argument.second)
            );
        }

        ionshared::Ptr<ionir::Prototype> ionIrPrototype = std::make_shared<ionir::Prototype>(
            node->name,
            ionIrArguments,
            ionIrReturnType,
            *this->buffers.module
        );

        this->constructStack.push(ionIrPrototype);
    }

    void IonIrLoweringPass::visitBlock(ionshared::Ptr<Block> node) {
        ionshared::Ptr<ionir::Function> ionIrFunctionBuffer = this->requireFunction();
        ionshared::OptPtr<ionir::FunctionBody> ionIrFunctionBody = std::nullopt;
        ionir::BasicBlockKind ionIrBasicBlockKind = ionir::BasicBlockKind::Internal;

        // TODO: Use a counter or something, along with naming depending on the block's parent (ex. if statement parent => if_0, etc.).
        std::string ionIrBasicBlockId = "tmp_block_" + std::to_string(this->getNameCounter());

        if (node->isFunctionBody()) {
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

        std::vector<ionshared::Ptr<Statement>> statements = node->statements;

        for (const auto &statement : statements) {
            // Visit the statement.
            this->visit(statement);

            // TODO: IMPORTANT: DO note that (some?) visitStatements, (ex. visitReturnStatement) already register insts via builder. (They use buffered builder, which is set on code above and bound to the buffered block, set above).
            // TODO: Insts and registers must be popped off and added onto the block.

            // Discard the result off the stack (if any), as it will not be used.
            this->constructStack.tryPop();
        }

        // Push the basic block onto the stack.
        if (!ionshared::util::hasValue(ionIrFunctionBody)) {
            this->constructStack.push(ionIrBasicBlock);
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

    void IonIrLoweringPass::visitIntegerLiteral(ionshared::Ptr<IntegerLiteral> node) {
        PtrResolvable<IntegerType> integerTypeResolvable = node->type;

        if (!integerTypeResolvable->isResolved()) {
            throw std::runtime_error("Type is unresolved");
        }

        ionshared::Ptr<Type> integerType = *node->type->getValue();

        if (integerType->typeKind != TypeKind::Integer) {
            throw std::runtime_error("Integer value's type must be integer type");
        }

        this->visit(integerType);

        ionshared::Ptr<ionir::IntegerType> ionIrIntegerType =
            this->typeStack.pop()->dynamicCast<ionir::IntegerType>();

        ionshared::Ptr<ionir::IntegerLiteral> ionIrIntegerLiteral =
            std::make_shared<ionir::IntegerLiteral>(ionIrIntegerType, node->value);

        this->constructStack.push(ionIrIntegerLiteral);
    }

    void IonIrLoweringPass::visitCharLiteral(ionshared::Ptr<CharLiteral> node) {
        ionshared::Ptr<ionir::CharLiteral> ionIrCharLiteral =
            std::make_shared<ionir::CharLiteral>(node->value);

        this->constructStack.push(ionIrCharLiteral);
    }

    void IonIrLoweringPass::visitStringLiteral(ionshared::Ptr<StringLiteral> node) {
        ionshared::Ptr<ionir::StringLiteral> ionIrStringLiteral =
            std::make_shared<ionir::StringLiteral>(node->value);

        this->constructStack.push(ionIrStringLiteral);
    }

    void IonIrLoweringPass::visitBooleanLiteral(ionshared::Ptr<BooleanLiteral> node) {
        ionshared::Ptr<ionir::BooleanLiteral> ionIrBooleanLiteral =
            std::make_shared<ionir::BooleanLiteral>(node->value);

        this->constructStack.push(ionIrBooleanLiteral);
    }

    void IonIrLoweringPass::visitGlobal(ionshared::Ptr<Global> node) {
        // Module buffer will be used, therefore it must be set.
        ionshared::Ptr<ionir::Module> ionIrModuleBuffer = this->requireModule();

        this->visit(node->type);

        ionshared::Ptr<ionir::Type> type = this->typeStack.pop();
        ionshared::OptPtr<Expression<>> nodeValue = node->value;
        ionshared::OptPtr<ionir::Value<>> value = std::nullopt;

        // Assign value if applicable.
        if (ionshared::util::hasValue(nodeValue)) {
            this->visit(*nodeValue);

            // Use static pointer cast when downcasting to ionir::Value<>.
            value = this->constructStack.pop()
                ->staticCast<ionir::Value<>>();

            // Ensure cast value is not nullptr as a precaution.
            if (!ionshared::util::hasValue(value)) {
                throw std::runtime_error("Value cast failed");
            }
        }

        ionshared::Ptr<ionir::Global> ionIrGlobalVariable =
            std::make_shared<ionir::Global>(type, node->name, value);

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

        this->constructStack.push(ionIrGlobalVariable);
    }

    void IonIrLoweringPass::visitIntegerType(ionshared::Ptr<IntegerType> node) {
        ionir::IntegerKind ionIrIntegerKind;

        /**
         * Create the corresponding IonIR integer type based off the
         * node's integer kind.
         */
        switch (node->integerKind) {
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

        this->typeStack.push(IonIrLoweringPass::processTypeQualifiers(
            std::make_shared<ionir::IntegerType>(ionIrIntegerKind, node->isSigned),
            node->qualifiers
        ));
    }

    void IonIrLoweringPass::visitBooleanType(ionshared::Ptr<BooleanType> node) {
        this->typeStack.push(IonIrLoweringPass::processTypeQualifiers(
            std::make_shared<ionir::BooleanType>(),
            node->qualifiers
        ));
    }

    void IonIrLoweringPass::visitVoidType(ionshared::Ptr<VoidType> node) {
        this->typeStack.push(IonIrLoweringPass::processTypeQualifiers(
            std::make_shared<ionir::VoidType>(),
            node->qualifiers
        ));
    }

    void IonIrLoweringPass::visitIfStatement(ionshared::Ptr<IfStatement> node) {
        ionshared::Ptr<ionir::BasicBlock> ionIrBasicBlockBuffer = this->requireBasicBlock();

        this->visit(node->condition);

        ionshared::Ptr<Block> parentBlock = node->getUnboxedParent();
        ionshared::Ptr<ionir::Construct> ionIrCondition = this->constructStack.pop();

        // TODO: Should verify if ionIrCondition is either Ref<> or Value<> here? Or in type-check?

        /**
         * Transfer all previously emitted instructions into a new continuation
         * block for the buffered block, then link the buffered block with the
         * new block.
         */
        uint32_t splitOrder = node->getOrder() + 1;

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
        successorBlock->parent = node;

        this->visitBlock(successorBlock);

        ionshared::Ptr<ionir::BasicBlock> ionIrSuccessorBasicBlock =
            this->constructStack.pop()->dynamicCast<ionir::BasicBlock>();

        this->visitBlock(node->consequentBlock);

        ionshared::Ptr<ionir::BasicBlock> ionIrConsequentBasicBlock =
            this->constructStack.pop()->dynamicCast<ionir::BasicBlock>();

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

        if (node->hasAlternativeBlock()) {
            this->visitBlock(*node->alternativeBlock);

            ionshared::Ptr<ionir::BasicBlock> ionIrAlternativeBlock =
                this->constructStack.pop()->dynamicCast<ionir::BasicBlock>();

            /**
             * Link the alternative basic block with the buffered basic
             * block's successor.
             */
            ionIrAlternativeBlock->link(ionIrSuccessorBasicBlock);
        }
    }

    void IonIrLoweringPass::visitReturnStatement(ionshared::Ptr<ReturnStatement> node) {
        ionshared::Ptr<ionir::InstBuilder> ionIrInstBuilder = this->requireBuilder();
        ionshared::OptPtr<ionir::Value<>> ionIrValue = std::nullopt;

        if (node->hasValue()) {
            this->visit(*node->value);

            // Use a static pointer cast to cast to ionir::Value<>.
            ionIrValue = this->constructStack.pop()->staticCast<ionir::Value<>>();

            // Verify cast result isn't nullptr as a precaution.
            if (!ionshared::util::hasValue(ionIrValue)) {
                throw std::runtime_error("Could not upcast return value");
            }
        }

        ionshared::Ptr<ionir::ReturnInst> ionIrReturnInst =
            ionIrInstBuilder->createReturn(ionIrValue);

        this->constructStack.push(ionIrReturnInst);
    }

    void IonIrLoweringPass::visitAssignmentStatement(ionshared::Ptr<AssignmentStatement> node) {
        ionshared::Ptr<ionir::InstBuilder> ionIrBuilderBuffer = this->requireBuilder();

        if (!node->variableDeclStatementRef->isResolved()) {
            // TODO: Better error.
            throw std::runtime_error("Expected variable declaration reference to be resolved");
        }
        else if (!this->symbolTable.contains(**node->variableDeclStatementRef)) {
            // TODO: Better error.
            throw std::runtime_error("Could not find corresponding IonIR alloca instruction on the symbol table");
        }

        ionshared::Ptr<VariableDeclStatement> variableDecl = **node->variableDeclStatementRef;

        ionshared::Ptr<ionir::AllocaInst> ionIrAllocaInst =
            *this->symbolTable.find<ionir::AllocaInst>(variableDecl);

        this->visit(node->value);

        ionIrBuilderBuffer->createStore(
            this->constructStack.pop()->staticCast<ionir::Value<>>(),
            ionIrAllocaInst
        );
    }

    void IonIrLoweringPass::visitVariableDecl(ionshared::Ptr<VariableDeclStatement> node) {
        this->requireBuilder();

        ionshared::Ptr<ionir::InstBuilder> ionIrInstBuilder = *this->buffers.builder;

        // First, visit the type and create a IonIR alloca inst,  and push it onto the stack.
        this->visit(node->type);

        ionshared::Ptr<ionir::Type> ionIrType = this->typeStack.pop();

        ionshared::Ptr<ionir::AllocaInst> ionIrAllocaInst =
            ionIrInstBuilder->createAlloca(node->name, ionIrType);

        this->symbolTable.set(node, ionIrAllocaInst);
        this->constructStack.push(ionIrAllocaInst);

        // Lastly, then create a IonIR store inst, and push it onto the stack.
        this->visit(node->value);

        // TODO: Value can be an expression as well. Check that ConstructKind == Value, otherwise handle appropriately.
        ionshared::Ptr<ionir::Value<>> ionIrValue =
            this->constructStack.pop()->staticCast<ionir::Value<>>();

        // TODO: Store inst not being pushed onto construct stack. Should it be pushed, or just AllocaInst?
        ionIrInstBuilder->createStore(
            ionIrValue,
            ionIrAllocaInst
        );
    }

    void IonIrLoweringPass::visitCallExpr(ionshared::Ptr<CallExpr> node) {
        PtrResolvable<> calleeRef = node->calleeResolvable;

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

        /**
         * Visit the callee it earlier if it hasn't been visited/emitted
         * at this point.
         */
        if (!this->symbolTable.contains(callee)) {
            this->lockBuffers([&, this] {
                this->visit(callee);
            });

            // TODO: The function/extern is being discarded here, but what if that function needs to be present in the stack because of the invoking method requires to pop it?
            this->constructStack.tryPop();
        }

        ionshared::OptPtr<ionir::Construct> ionIrCalleeResult =
            this->symbolTable.find(callee);

        if (!ionshared::util::hasValue(ionIrCalleeResult)) {
            throw std::runtime_error("Corresponding emitted IonIR entity could not be found on symbol table");
        }

        std::vector<ionshared::Ptr<ionir::Construct>> ionIrArgs = {};

        for (const auto& arg : node->args) {
            arg->accept(*this);
            ionIrArgs.push_back(this->constructStack.pop());
        }

        /**
         * At this point, we know that the basic block buffer is
         * set because the builder buffer is set.
         */
        ionshared::Ptr<ionir::CallInst> ionIrCallInst = this->requireBuilder()->createCall(
            *ionIrCalleeResult,
            ionIrArgs
        );

        this->constructStack.push(ionIrCallInst);
    }

    void IonIrLoweringPass::visitOperationExpr(ionshared::Ptr<OperationExpr> node) {
        std::optional<ionir::OperatorKind> ionIrOperatorKindResult =
            util::findIonIrOperatorKind(node->operation);

        if (!ionIrOperatorKindResult.has_value()) {
            throw std::runtime_error("Unknown intrinsic operator kind");
        }

        this->visit(node->leftSideValue);

        ionshared::Ptr<ionir::Value<>> ionIrLeftSideValue =
            this->constructStack.pop()->staticCast<ionir::Value<>>();

        ionshared::OptPtr<ionir::Value<>> ionIrRightSideValue = std::nullopt;

        if (ionshared::util::hasValue(node->rightSideValue)) {
            this->visit(*node->rightSideValue);

            ionIrRightSideValue =
                this->constructStack.pop()->staticCast<ionir::Value<>>();
        }

        this->constructStack.push(std::make_shared<ionir::OperationValue>(
            *ionIrOperatorKindResult,
            ionIrLeftSideValue,
            ionIrRightSideValue
        ));
    }

    void IonIrLoweringPass::visitStruct(ionshared::Ptr<Struct> node) {
        ionshared::Ptr<ionir::Module> ionIrModuleBuffer = this->requireModule();
        ionir::Scope globalSymbolTable = ionIrModuleBuffer->context->getGlobalScope();

        if (globalSymbolTable->contains(node->name)) {
            // TODO: Use DiagnosticBuilder.
            throw std::runtime_error("Struct was already previously defined in the module");
        }

        auto fieldsMap = node->fields->unwrap();

        ionir::Fields ionIrFields =
            ionshared::util::makePtrSymbolTable<ionir::Type>();

        for (const auto& [name, type] : fieldsMap) {
            this->visit(type);
            ionIrFields->set(name, this->typeStack.pop());
        }

        ionshared::Ptr<ionir::Struct> ionIrStruct =
            std::make_shared<ionir::Struct>(node->name, ionIrFields);

        ionIrModuleBuffer->context->getGlobalScope()->set(
            node->name,
            ionIrStruct
        );

        this->constructStack.push(ionIrStruct);
    }
}
