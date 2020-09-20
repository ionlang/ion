#include <ionshared/misc/util.h>
#include <ionir/construct/value/integer_literal.h>
#include <ionir/construct/value/char_literal.h>
#include <ionir/construct/value/string_literal.h>
#include <ionir/construct/value/boolean_literal.h>
#include <ionir/construct/function.h>
#include <ionir/construct/global.h>
#include <ionir/construct/extern.h>
#include <ionir/misc/inst_builder.h>
#include <ionir/const/const.h>
#include <ionlang/passes/lowering/ionir_lowering_pass.h>
#include <ionlang/const/notice.h>
#include <ionlang/const/const.h>

namespace ionlang {
    ionshared::Ptr<ionir::Module> IonIrLoweringPass::requireModule() {
        if (!ionshared::util::hasValue(this->moduleBuffer)) {
            throw std::runtime_error("Expected the module buffer to be set, but was nullptr");
        }

        return *this->moduleBuffer;
    }

    ionshared::Ptr<ionir::Function> IonIrLoweringPass::requireFunction() {
        if (!ionshared::util::hasValue(this->functionBuffer)) {
            throw std::runtime_error("Expected the function buffer to be set, but was nullptr");
        }

        return *this->functionBuffer;
    }

    ionshared::Ptr<ionir::BasicBlock> IonIrLoweringPass::requireBasicBlock() {
        if (!ionshared::util::hasValue(this->basicBlockBuffer)) {
            throw std::runtime_error("Expected the basic block buffer to be set, but was nullptr");
        }

        return *this->basicBlockBuffer;
    }

    ionshared::Ptr<ionir::InstBuilder> IonIrLoweringPass::requireBuilder() {
        // Builder must be instantiated.
        if (!this->builderBuffer.has_value()) {
            // Otherwise, throw a runtime error.
            throw std::runtime_error("Expected builder to be instantiated");
        }

        return *builderBuffer;
    }

    void IonIrLoweringPass::setBuilder(ionshared::Ptr<ionir::BasicBlock> basicBlock) {
        this->builderBuffer = basicBlock->createBuilder();
        this->basicBlockBuffer = basicBlock;
    }

    void IonIrLoweringPass::lockBuffers(const std::function<void()> &callback) {
        auto ionIrModuleBuffer = this->requireModule();
        auto ionIrFunctionBuffer = this->requireFunction();
        ionshared::Ptr<ionir::BasicBlock> ionIrBasicBlockBuffer = this->requireBasicBlock();

        callback();
        this->moduleBuffer = ionIrModuleBuffer;
        this->functionBuffer = ionIrFunctionBuffer;
        this->setBuilder(ionIrBasicBlockBuffer);
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
        moduleBuffer(std::nullopt),
        functionBuffer(std::nullopt),
        basicBlockBuffer(std::nullopt),
        builderBuffer(std::nullopt),
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
        return this->moduleBuffer;
    }

    bool IonIrLoweringPass::setModuleBuffer(const std::string &id) {
        if (this->modules->contains(id)) {
            this->moduleBuffer = this->modules->lookup(id);

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
        this->moduleBuffer = std::make_shared<ionir::Module>(node->getId());

        // Set the module on the modules symbol table.
        this->modules->set(node->getId(), *this->moduleBuffer);

        // Proceed to visit all the module's children (top-level constructs).
        std::map<std::string, ionshared::Ptr<Construct>> moduleSymbolTable =
            node->getContext()->getGlobalScope()->unwrap();

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
        /**
         * Function was already emitted, do not continue. This check
         * is present because visiting a call expression referencing a
         * function that will be processed in the future, no symbol has
         * been emitted on the local symbol table. So that function (this
         * node) is either emitted here or when emitting the call expression,
         * in other words, whenever first encountered or needed.
         */
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

        std::string ionIrFunctionId = node->getPrototype()->getId();

        if (moduleBuffer->getContext()->getGlobalScope()->contains(ionIrFunctionId)) {
            throw ionshared::util::quickError(
                IONLANG_NOTICE_FUNCTION_ALREADY_DEFINED,
                ionIrFunctionId
            );
        }

        this->visitPrototype(node->getPrototype());

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
        this->functionBuffer = ionIrFunction;

        this->visitBlock(node->getBody());

        // TODO: Redundant Repetitive assignment?
        // Set the function buffer.
        this->functionBuffer = ionIrFunction;

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
        ionshared::Ptr<ionir::Module> moduleBuffer = this->requireModule();

        ionshared::PtrSymbolTable<ionir::Construct> moduleBufferSymbolTable =
            moduleBuffer->getContext()->getGlobalScope();

        ionshared::Ptr<Prototype> prototype = node->getPrototype();

        // TODO: Use verify instead.
        if (prototype == nullptr) {
            throw std::runtime_error("Unexpected external definition's prototype to be null");
        }

        if (moduleBufferSymbolTable->contains(prototype->getId())) {
            throw std::runtime_error("Entity with same id already exists on module");
        }

        this->visitPrototype(prototype);

        ionshared::Ptr<ionir::Prototype> ionIrPrototype =
            this->constructStack.pop()->dynamicCast<ionir::Prototype>();

        ionshared::Ptr<ionir::Extern> ionIrExtern =
            std::make_shared<ionir::Extern>(ionIrPrototype);

        /**
         * Register the IonIR extern on the module buffer's symbol table.
         * This will allow the IonIR codegen pass to visit the extern.
         */
        moduleBufferSymbolTable->set(prototype->getId(), ionIrExtern);

        this->constructStack.push(ionIrExtern);
    }

    void IonIrLoweringPass::visitPrototype(ionshared::Ptr<Prototype> node) {
        this->requireModule();
        this->visitType(node->getReturnType());

        ionshared::Ptr<ionir::Type> ionIrReturnType = this->typeStack.pop();
        ionshared::Ptr<ionir::Args> ionIrArguments = std::make_shared<ionir::Args>();
        ionshared::Ptr<Args> arguments = node->getArguments();
        auto nativeArguments = arguments->getItems()->unwrap();

        ionIrArguments->setIsVariable(arguments->getIsVariable());

        // TODO: Should Args be a construct, and be visited?
        for (const auto &[id, argument] : nativeArguments) {
            this->visitType(argument.first);

            ionIrArguments->getItems()->set(
                argument.second,
                std::make_pair(this->typeStack.pop(), argument.second)
            );
        }

        ionshared::Ptr<ionir::Prototype> ionIrPrototype = std::make_shared<ionir::Prototype>(
            node->getId(),
            ionIrArguments,
            ionIrReturnType,
            *this->moduleBuffer
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
            ionIrFunctionBuffer->setBody(*ionIrFunctionBody);

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
                ionIrFunctionBuffer->getBody(),
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

        std::vector<ionshared::Ptr<Statement>> statements = node->getStatements();

        for (const auto &statement : statements) {
            // Visit the statement.
            this->visitStatement(statement);

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
        ionshared::Ptr<Type> nodeType = node->getType();

        if (nodeType->getTypeKind() != TypeKind::Integer) {
            throw std::runtime_error("Integer value's type must be integer type");
        }

        this->visitIntegerType(nodeType->dynamicCast<IntegerType>());

        ionshared::Ptr<ionir::IntegerType> ionIrIntegerType =
            this->typeStack.pop()->dynamicCast<ionir::IntegerType>();

        ionshared::Ptr<ionir::IntegerLiteral> ionIrIntegerLiteral =
            std::make_shared<ionir::IntegerLiteral>(ionIrIntegerType, node->getValue());

        // Use static pointer cast when downcasting to ionir::Value<>.
        this->constructStack.push(ionIrIntegerLiteral->staticCast<ionir::Value<>>());
    }

    void IonIrLoweringPass::visitCharLiteral(ionshared::Ptr<CharLiteral> node) {
        ionshared::Ptr<ionir::CharLiteral> ionIrCharLiteral =
            std::make_shared<ionir::CharLiteral>(node->getValue());

        this->constructStack.push(ionIrCharLiteral->dynamicCast<ionir::Value<>>());
    }

    void IonIrLoweringPass::visitStringLiteral(ionshared::Ptr<StringLiteral> node) {
        ionshared::Ptr<ionir::StringLiteral> ionIrStringLiteral =
            std::make_shared<ionir::StringLiteral>(node->getValue());

        this->constructStack.push(ionIrStringLiteral->dynamicCast<ionir::Value<>>());
    }

    void IonIrLoweringPass::visitBooleanLiteral(ionshared::Ptr<BooleanLiteral> node) {
        ionshared::Ptr<ionir::BooleanLiteral> ionIrBooleanLiteral =
            std::make_shared<ionir::BooleanLiteral>(node->getValue());

        this->constructStack.push(ionIrBooleanLiteral->dynamicCast<ionir::Value<>>());
    }

    void IonIrLoweringPass::visitGlobal(ionshared::Ptr<Global> node) {
        // Module buffer will be used, therefore it must be set.
        ionshared::Ptr<ionir::Module> ionIrModuleBuffer = this->requireModule();

        this->visitType(node->getType());

        ionshared::Ptr<ionir::Type> type = this->typeStack.pop();
        ionshared::OptPtr<Value<>> nodeValue = node->getValue();
        ionshared::OptPtr<ionir::Value<>> value = std::nullopt;

        // Assign value if applicable.
        if (ionshared::util::hasValue(nodeValue)) {
            Pass::visitValue(*nodeValue);

            // Use static pointer cast when downcasting to ionir::Value<>.
            value = this->constructStack.pop()->staticCast<ionir::Value<>>();

            // Ensure cast value is not nullptr as a precaution.
            if (!ionshared::util::hasValue(value)) {
                throw std::runtime_error("Value cast failed");
            }
        }

        ionshared::Ptr<ionir::Global> ionIrGlobalVariable =
            std::make_shared<ionir::Global>(type, node->getId(), value);

        /**
         * Register the global variable on the buffered module's symbol table.
         * This will allow it to be visited and emitted to LLVM IR during the
         * IonIR codegen phase.
         */
        ionIrModuleBuffer->getContext()->getGlobalScope()->set(
            ionIrGlobalVariable->getId(),
            ionIrGlobalVariable
        );

        this->constructStack.push(ionIrGlobalVariable);
    }

    void IonIrLoweringPass::visitType(ionshared::Ptr<Type> node) {
        // Convert type to a pointer if applicable.
        // TODO: Now it's PointerType (soon to be implemented or already).
        //        if (node->getIsPointer()) {
        //            /**
        //             * TODO: Convert type to pointer before passing on
        //             * to explicit handlers, thus saving time and code.
        //             */
        //        }

        switch (node->getTypeKind()) {
            case TypeKind::Void: {
                return this->visitVoidType(node->staticCast<VoidType>());
            }

            case TypeKind::Integer: {
                return this->visitIntegerType(node->staticCast<IntegerType>());
            }

            case TypeKind::Boolean: {
                return this->visitBooleanType(node->staticCast<BooleanType>());
            }

            case TypeKind::String: {
                // TODO

                throw std::runtime_error("Not implemented");
            }

            case TypeKind::UserDefined: {
                // TODO

                throw std::runtime_error("Not implemented");
            }

            default: {
                throw std::runtime_error("Could not identify type kind");
            }
        }
    }

    void IonIrLoweringPass::visitIntegerType(ionshared::Ptr<IntegerType> node) {
        ionir::IntegerKind ionIrIntegerKind;

        /**
         * Create the corresponding IonIR integer type based off the
         * node's integer kind.
         */
        switch (node->getIntegerKind()) {
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

        ionshared::Ptr<ionir::IntegerType> ionIrType =
            std::make_shared<ionir::IntegerType>(ionIrIntegerKind);

        this->typeStack.push(ionIrType);
    }

    void IonIrLoweringPass::visitBooleanType(ionshared::Ptr<BooleanType> node) {
        ionshared::Ptr<ionir::BooleanType> ionIrBooleanType =
            std::make_shared<ionir::BooleanType>();

        this->typeStack.push(ionIrBooleanType);
    }

    void IonIrLoweringPass::visitVoidType(ionshared::Ptr<VoidType> node) {
        ionshared::Ptr<ionir::VoidType> ionIrVoidType =
            std::make_shared<ionir::VoidType>();

        this->typeStack.push(ionIrVoidType);
    }

    void IonIrLoweringPass::visitIfStatement(ionshared::Ptr<IfStatement> node) {
        ionshared::Ptr<ionir::BasicBlock> ionIrBasicBlockBuffer = this->requireBasicBlock();

        Pass::visit(node->getCondition());

        ionshared::Ptr<Block> parentBlock = node->getParent();
        ionshared::Ptr<ionir::Construct> ionIrCondition = this->constructStack.pop();

        // TODO: Should verify if ionIrCondition is either Ref<> or Value<> here? Or in typecheck?

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
        if (parentBlock->getStatements().size() - 1 >= splitOrder) {
            // TODO: Symbol table not being migrated (split does not do this).
            successorBlock = parentBlock->split(splitOrder);
        }
        /**
         * There are no more upcoming statements after this if statement.
         * Simply create an empty block, with the same parent.
         */
        else {
            // TODO: Symbol table not being migrated.
            successorBlock = std::make_shared<Block>(parentBlock->getParent());
        }

        // TODO: Hotfix to avoid function body (thus overriding bufferFunction's body).
        successorBlock->setParent(node);

        this->visitBlock(successorBlock);

        ionshared::Ptr<ionir::BasicBlock> ionIrSuccessorBasicBlock =
            this->constructStack.pop()->dynamicCast<ionir::BasicBlock>();

        this->visitBlock(node->getConsequentBlock());

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
            this->visitBlock(*node->getAlternativeBlock());

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
            Pass::visit(*node->getValue());

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

        if (!node->getVariableDeclStatement()->isResolved()) {
            // TODO: Better error.
            throw std::runtime_error("Expected variable declaration reference to be resolved");
        }
        else if (!this->symbolTable.contains(*node->getVariableDeclStatement()->getValue())) {
            // TODO: Better error.
            throw std::runtime_error("Could not find corresponding IonIR alloca instruction on the symbol table");
        }

        ionshared::Ptr<VariableDeclStatement> variableDecl = *node->getVariableDeclStatement()->getValue();

        ionshared::Ptr<ionir::AllocaInst> ionIrAllocaInst =
            *this->symbolTable.find<ionir::AllocaInst>(variableDecl);

        this->visit(node->getValue());

        ionIrBuilderBuffer->createStore(
            this->constructStack.pop()->staticCast<ionir::Value<>>(),
            ionIrAllocaInst
        );
    }

    void IonIrLoweringPass::visitVariableDecl(ionshared::Ptr<VariableDeclStatement> node) {
        this->requireBuilder();

        ionshared::Ptr<ionir::InstBuilder> ionIrInstBuilder = *this->builderBuffer;

        // First, visit the type and create a IonIR alloca inst,  and push it onto the stack.
        this->visitType(node->getType());

        ionshared::Ptr<ionir::Type> ionIrType = this->typeStack.pop();

        ionshared::Ptr<ionir::AllocaInst> ionIrAllocaInst =
            ionIrInstBuilder->createAlloca(node->getId(), ionIrType);

        this->symbolTable.set(node, ionIrAllocaInst);
        this->constructStack.push(ionIrAllocaInst);

        // Lastly, then create a IonIR store inst, and push it onto the stack.
        Pass::visit(node->getValue());

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
        ionshared::Ptr<ionir::InstBuilder> ionIrInstBuilder = this->requireBuilder();
        PtrRef<Function> calleeRef = node->getCalleeRef();

        if (!calleeRef->isResolved()) {
            throw std::runtime_error("Expected callee function reference to be resolved");
        }

        ionshared::Ptr<Function> callee = *calleeRef->getValue();

        /**
         * Visit the callee function earlier if it hasn't been
         * visited/emitted at this point.
         */
        if (!this->symbolTable.contains(callee)) {
            this->lockBuffers([&, this] {
                this->visitFunction(callee);
            });

            // TODO: The function is being discarded here, but what if that function needs to be present in the stack because of the invoking method requires to pop it?
            this->constructStack.tryPop();
        }

        ionshared::OptPtr<ionir::Function> ionIrCalleeResult =
            this->symbolTable.find<ionir::Function>(callee);

        if (!ionshared::util::hasValue(ionIrCalleeResult)) {
            throw std::runtime_error("Corresponding emitted IonIR entity could not be found on symbol table");
        }

        /**
         * At this point, we know that the basic block buffer is
         * set because the builder buffer is set.
         */
        ionshared::Ptr<ionir::CallInst> ionIrCallInst = ionIrInstBuilder->createCall(
            *ionIrCalleeResult
            // TODO: Process arguments.
        );

        this->constructStack.push(ionIrCallInst);
    }
}
