#include <ionshared/misc/util.h>
#include <ionir/construct/value/integer_value.h>
#include <ionir/construct/value/char_value.h>
#include <ionir/construct/value/string_value.h>
#include <ionir/construct/value/boolean_value.h>
#include <ionir/construct/function.h>
#include <ionir/construct/global.h>
#include <ionir/construct/extern.h>
#include <ionir/misc/inst_builder.h>
#include <ionir/const/const.h>
#include <ionlang/passes/codegen/ionir_codegen_pass.h>
#include <ionlang/const/notice.h>
#include <ionlang/const/const.h>

namespace ionlang {
    ionshared::Ptr<ionir::Module> IonIrCodegenPass::requireModule() {
        if (!ionshared::Util::hasValue(this->moduleBuffer)) {
            throw std::runtime_error("Expected the module buffer to be set, but was null");
        }

        return *this->moduleBuffer;
    }

    void IonIrCodegenPass::requireFunction() {
        if (!ionshared::Util::hasValue(this->functionBuffer)) {
            throw std::runtime_error("Expected the function buffer to be set, but was null");
        }
    }

    ionshared::Ptr<ionir::InstBuilder> IonIrCodegenPass::requireBuilder() {
        // Builder must be instantiated.
        if (!this->builderBuffer.has_value()) {
            // Otherwise, throw a runtime error.
            throw std::runtime_error("Expected builder to be instantiated");
        }

        return *builderBuffer;
    }

    void IonIrCodegenPass::setBuilder(ionshared::Ptr<ionir::BasicBlock> basicBlock) {
        this->builderBuffer = basicBlock->createBuilder();
        this->basicBlockBuffer = basicBlock;
    }

    IonIrCodegenPass::IonIrCodegenPass(ionshared::PtrSymbolTable<ionir::Module> modules)
        : modules(std::move(modules)), constructStack(), typeStack(), moduleBuffer(std::nullopt), functionBuffer(std::nullopt), basicBlockBuffer(std::nullopt), builderBuffer(std::nullopt) {
        //
    }

    IonIrCodegenPass::~IonIrCodegenPass() {
        // TODO
    }

    ionshared::Stack<ionshared::Ptr<ionir::Construct>> IonIrCodegenPass::getConstructStack() const noexcept {
        return this->constructStack;
    }

    ionshared::Stack<ionshared::Ptr<ionir::Type>> IonIrCodegenPass::getTypeStack() const noexcept {
        return this->typeStack;
    }

    ionshared::Ptr<ionshared::SymbolTable<ionshared::Ptr<ionir::Module>>> IonIrCodegenPass::getModules() const {
        return this->modules;
    }

    ionshared::OptPtr<ionir::Module> IonIrCodegenPass::getModuleBuffer() const {
        return this->moduleBuffer;
    }

    bool IonIrCodegenPass::setModuleBuffer(const std::string &id) {
        if (this->modules->contains(id)) {
            this->moduleBuffer = this->modules->lookup(id);

            return true;
        }

        return false;
    }

    void IonIrCodegenPass::visit(ionshared::Ptr<Construct> node) {
        /**
         * Only instruct the node to visit this instance and
         * not its children, since they're already visited by
         * the other member methods.
         */
        node->accept(*this);
    }

    void IonIrCodegenPass::visitModule(ionshared::Ptr<Module> node) {
        this->moduleBuffer = std::make_shared<ionir::Module>(node->getId());

        // Set the module on the modules symbol table.
        this->modules->insert(node->getId(), *this->moduleBuffer);

        // Proceed to visit all the module's children (top-level constructs).
        std::map<std::string, ionshared::Ptr<Construct>> moduleSymbolTable =
            node->getSymbolTable()->unwrap();

        for (const auto &[id, topLevelConstruct] : moduleSymbolTable) {
            this->visit(topLevelConstruct);
        }
    }

    void IonIrCodegenPass::visitFunction(ionshared::Ptr<Function> node) {
        ionshared::Ptr<ionir::Module> moduleBuffer = this->requireModule();

        // TODO: Awaiting verification implementation.
//        if (!node->verify()) {
//            throw ionshared::Util::quickError(
//                IONLANG_NOTICE_MISC_VERIFICATION_FAILED,
//                "Function" // TODO: Hard-coded, should be using Util::getConstructName().
//            );
//        }

        std::string ionIrFunctionId = node->getPrototype()->getId();

        if (moduleBuffer->getSymbolTable()->contains(ionIrFunctionId)) {
            throw ionshared::Util::quickError(
                IONLANG_NOTICE_FUNCTION_ALREADY_DEFINED,
                ionIrFunctionId
            );
        }

        this->visitPrototype(node->getPrototype());

        ionshared::Ptr<ionir::Prototype> ionIrPrototype =
            this->constructStack.pop()->dynamicCast<ionir::Prototype>();

        this->visitBlock(node->getBody());

        ionshared::Ptr<ionir::FunctionBody> ionIrFunctionBody =
            this->constructStack.pop()->dynamicCast<ionir::FunctionBody>();

        // Retrieve the resulting function off the stack.
        ionshared::Ptr<ionir::Function> ionIrFunction =
            std::make_shared<ionir::Function>(ionIrPrototype, ionIrFunctionBody);

        // The function's body basic block's parent is nullptr. At this stage, fill it in.
        ionIrFunctionBody->setParent(ionIrFunction);

        // Set the function buffer.
        this->functionBuffer = ionIrFunction;

        // Register the newly created function on the buffer module's symbol table.
        moduleBuffer->insertFunction(ionIrFunction);

        // Push the function back onto the stack.
        this->constructStack.push(ionIrFunction);
    }

    void IonIrCodegenPass::visitExtern(ionshared::Ptr<Extern> node) {
        ionshared::Ptr<ionir::Module> moduleBuffer = this->requireModule();

        ionshared::PtrSymbolTable<ionir::Construct> moduleBufferSymbolTable =
            moduleBuffer->getSymbolTable();

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
        moduleBufferSymbolTable->insert(prototype->getId(), ionIrExtern);

        this->constructStack.push(ionIrExtern);
    }

    void IonIrCodegenPass::visitPrototype(ionshared::Ptr<Prototype> node) {
        this->requireModule();

        // Retrieve argument count from the argument vector.
        uint32_t argumentCount = node->getArgs()->getItems().getSize();

        // Create the argument buffer vector.
        ionshared::Ptr<ionir::Args> arguments = std::make_shared<ionir::Args>();

        // TODO: Process arguments.
        //            for (uint32_t i = 0; i < argumentCount; ++i) {
        //                arguments.push_back(llvm::Type::getDoubleTy(**this->contextBuffer));
        //            }

        // Visit and pop the return type.
        this->visitType(node->getReturnType());

        ionshared::Ptr<ionir::Type> ionIrReturnType = this->typeStack.pop();

        // TODO: Arguments are just hard-coded empty, they should be precessed before then popped somehow then passed on the make_shared() below.
        auto tempEmptyArgs = std::make_shared<ionir::Args>();

        // TODO: Support for variable arguments.
        // TODO: Args in form of IonIR entities (need to create & implement this->visitArgs() or is it this->visitArg() with a loop here? Investigate.).
        ionshared::Ptr<ionir::Prototype> ionIrPrototype =
            std::make_shared<ionir::Prototype>(node->getId(), tempEmptyArgs, ionIrReturnType, *this->moduleBuffer);

        // TODO: Actually process args.
//        uint32_t i = 0;
//        auto args = ionIrFunction->get()->getPrototype()->getArgs()->getItems().unwrap();
//
//        for (const auto &arg : args) {
//            // TODO: getItems() no longer a vector; cannot index by index, only key.
//            // Retrieve the name element from the argument tuple.
//            //            std::string name = node->getArgs()->getItems()[i].second;
//
//            // Name the argument.
//            //            arg.setName(name);
//
//            // Increment the counter to prepare for next iteration.
//            i++;
//        }

        this->constructStack.push(ionIrPrototype);
    }

    void IonIrCodegenPass::visitBlock(ionshared::Ptr<Block> node) {
        this->requireFunction();

        ionshared::OptPtr<ionir::FunctionBody> ionIrFunctionBody = std::nullopt;
        ionir::BasicBlockKind ionIrBasicBlockKind = ionir::BasicBlockKind::Internal;

        // TODO: Use a counter or something, along with naming depending on the block's parent (ex. if statement parent => if_0, etc.).
        std::string ionIrBasicBlockId = "FIXME_NAME";

        if (node->isFunctionBody()) {
            // TODO: Make function body and push it onto the stack.
            ionIrFunctionBody = std::make_shared<ionir::FunctionBody>();
            ionIrBasicBlockKind = ionir::BasicBlockKind::Entry;
            ionIrBasicBlockId = ionir::Const::basicBlockEntryId;
        }

        // TODO: Create the block somehow. It requires 'BasicBlockKind', 'registers' and 'insts'.
        ionshared::Ptr<ionir::BasicBlock> ionIrBasicBlock =
            std::make_shared<ionir::BasicBlock>(ionir::BasicBlockOpts{
                // The basic block's parent will be filled in by the calling visit function.
                nullptr,

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

            // Discard the result off the stack, as it will not be used.
            this->constructStack.pop();
        }

        // Push the basic block onto the stack.
        if (!ionshared::Util::hasValue(ionIrFunctionBody)) {
            this->constructStack.push(ionIrBasicBlock);
        }
        /**
         * Provided block classifies as a function body. Push the resulting
         * function body onto the stack instead.
         */
        else {
            // Register the IonIR basic block on the function body's symbol table.
            ionIrFunctionBody->get()->getSymbolTable()->insert(
                ionir::Const::basicBlockEntryId,
                ionIrBasicBlock
            );

            this->constructStack.push(*ionIrFunctionBody);
        }
    }

    void IonIrCodegenPass::visitIntegerValue(ionshared::Ptr<IntegerValue> node) {
        ionshared::Ptr<Type> nodeType = node->getType();

        if (nodeType->getTypeKind() != TypeKind::Integer) {
            throw std::runtime_error("Integer value's type must be integer type");
        }

        this->visitIntegerType(nodeType->dynamicCast<IntegerType>());

        ionshared::Ptr<ionir::IntegerType> ionIrIntegerType = this->typeStack.pop()->dynamicCast<ionir::IntegerType>();

        ionshared::Ptr<ionir::IntegerValue> ionIrIntegerValue =
            std::make_shared<ionir::IntegerValue>(ionIrIntegerType, node->getValue());

        this->constructStack.push(ionIrIntegerValue->dynamicCast<ionir::Value<>>());
    }

    void IonIrCodegenPass::visitCharValue(ionshared::Ptr<CharValue> node) {
        ionshared::Ptr<ionir::CharValue> ionIrCharValue =
            std::make_shared<ionir::CharValue>(node->getValue());

        this->constructStack.push(ionIrCharValue->dynamicCast<ionir::Value<>>());
    }

    void IonIrCodegenPass::visitStringValue(ionshared::Ptr<StringValue> node) {
        ionshared::Ptr<ionir::StringValue> ionIrStringValue =
            std::make_shared<ionir::StringValue>(node->getValue());

        this->constructStack.push(ionIrStringValue->dynamicCast<ionir::Value<>>());
    }

    void IonIrCodegenPass::visitBooleanValue(ionshared::Ptr<BooleanValue> node) {
        ionshared::Ptr<ionir::BooleanValue> ionIrBooleanValue =
            std::make_shared<ionir::BooleanValue>(node->getValue());

        this->constructStack.push(ionIrBooleanValue->dynamicCast<ionir::Value<>>());
    }

    void IonIrCodegenPass::visitGlobal(ionshared::Ptr<Global> node) {
        // Module buffer will be used, therefore it must be set.
        this->requireModule();

        this->visitType(node->getType());

        ionshared::Ptr<ionir::Type> type = this->typeStack.pop();
        ionshared::OptPtr<Value<>> nodeValue = node->getValue();
        ionshared::OptPtr<ionir::Value<>> value = std::nullopt;

        // Assign value if applicable.
        if (ionshared::Util::hasValue(nodeValue)) {
            // Visit global variable value.
            this->visitValue(*nodeValue);

            value = this->constructStack.pop()->dynamicCast<ionir::Value<>>();
        }

        ionshared::Ptr<ionir::Global> globalVar =
            std::make_shared<ionir::Global>(type, node->getId(), value);

        this->constructStack.push(globalVar);
    }

    void IonIrCodegenPass::visitType(ionshared::Ptr<Type> node) {
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

    void IonIrCodegenPass::visitIntegerType(ionshared::Ptr<IntegerType> node) {
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

    void IonIrCodegenPass::visitBooleanType(ionshared::Ptr<BooleanType> node) {
        ionshared::Ptr<ionir::BooleanType> ionIrBooleanType =
            std::make_shared<ionir::BooleanType>();

        this->typeStack.push(ionIrBooleanType);
    }

    void IonIrCodegenPass::visitVoidType(ionshared::Ptr<VoidType> node) {
        ionshared::Ptr<ionir::VoidType> ionIrVoidType =
            std::make_shared<ionir::VoidType>();

        this->typeStack.push(ionIrVoidType);
    }

    void IonIrCodegenPass::visitIfStatement(ionshared::Ptr<IfStatement> node) {
        // TODO: Implement.
        throw std::runtime_error("Not implemented");
    }

    void IonIrCodegenPass::visitReturnStatement(ionshared::Ptr<ReturnStatement> node) {
        ionshared::Ptr<ionir::InstBuilder> ionIrInstBuilder = this->requireBuilder();
        ionshared::OptPtr<ionir::Value<>> ionIrValue = std::nullopt;

        if (node->hasValue()) {
            this->visitValue(*node->getValue());
            ionIrValue = this->constructStack.pop()->dynamicCast<ionir::Value<>>();
        }

        ionshared::Ptr<ionir::ReturnInst> ionIrReturnInst =
            ionIrInstBuilder->createReturn(ionIrValue);

        this->constructStack.push(ionIrReturnInst);
    }

    void IonIrCodegenPass::visitVariableDecl(ionshared::Ptr<VariableDecl> node) {
        this->requireBuilder();

        ionshared::Ptr<ionir::InstBuilder> ionIrInstBuilder = *this->builderBuffer;

        // First, visit the type and create a IonIR alloca inst,  and push it onto the stack.
        this->visitType(node->getType());

        ionshared::Ptr<ionir::Type> ionIrType = this->typeStack.pop();

        ionshared::Ptr<ionir::AllocaInst> ionIrAllocaInst =
            ionIrInstBuilder->createAlloca(node->getId(), ionIrType);

        this->constructStack.push(ionIrAllocaInst);

        // Lastly, then create a IonIR store inst, and push it onto the stack.
        this->visitValue(node->getValue());

        ionshared::Ptr<ionir::Value<>> ionIrValue =
            this->constructStack.pop()->dynamicCast<ionir::Value<>>();

        ionir::PtrRef<ionir::AllocaInst> ionIrRef = std::make_shared<ionir::Ref<ionir::AllocaInst>>(
            node->getId(),

            // TODO: CRITICAL: Provide owner for ref!
            nullptr,

            ionIrAllocaInst
        );

        ionshared::Ptr<ionir::StoreInst> ionIrStoreInst = ionIrInstBuilder->createStore(
            ionIrValue,
            ionIrRef
        );
    }

    void IonIrCodegenPass::visitCallStatement(ionshared::Ptr<CallStatement> node) {
        this->requireBuilder();

        ionshared::Ptr<ionir::InstBuilder> ionIrInstBuilder = *this->builderBuffer;

        // TODO: Continue/finish implementation.
//        ionIrInstBuilder->createCall()

        throw std::runtime_error("Not implemented");
    }
}
