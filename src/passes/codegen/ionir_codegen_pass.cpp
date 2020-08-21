#include <ionshared/misc/util.h>
#include <ionir/construct/function.h>
#include <ionir/construct/value/integer_value.h>
#include <ionir/construct/value/char_value.h>
#include <ionir/construct/value/string_value.h>
#include <ionir/construct/value/boolean_value.h>
#include <ionlang/passes/codegen/ionir_codegen_pass.h>
#include <ionlang/const/notice.h>

namespace ionlang {
    IonIrCodegenPass::IonIrCodegenPass() = default;

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

    void IonIrCodegenPass::visit(ionshared::Ptr<Construct> node) {
        /**
         * Only instruct the node to visit this instance and
         * not its children, since they're already visited by
         * the other member methods.
         */
        node->accept(*this);
    }

    void IonIrCodegenPass::visitExtern(ionshared::Ptr<Extern> node) {
        if (node->getPrototype() == nullptr) {
            throw std::runtime_error("Unexpected external definition's prototype to be null");
        }

        ionshared::OptPtr<ionir::Function> existingDefinition =
            moduleBuffer->get()->lookupFunction(node->getPrototype()->getId());

        if (ionshared::Util::hasValue(existingDefinition)) {
            throw std::runtime_error("Re-definition of extern not allowed");
        }

        // Visit the prototype.
        this->visitPrototype(node->getPrototype());

        // No need to push the resulting function onto the stack.
    }

    void IonIrCodegenPass::visitPrototype(ionshared::Ptr<Prototype> node) {
        // Retrieve argument count from the argument vector.
        uint32_t argumentCount = node->getArgs()->getItems().getSize();

        // Create the argument buffer vector.
        ionshared::Ptr<ionir::Args> arguments = std::make_shared<ionir::Args>();

        // Attempt to retrieve an existing function.
        ionshared::OptPtr<ionir::Function> ionIrFunction =
            this->moduleBuffer->get()->lookupFunction(node->getId());

        // A function with a matching identifier already exists.
        if (ionshared::Util::hasValue(ionIrFunction)) {
            ionshared::Ptr<ionir::Prototype> ionIrPrototype = ionIrFunction->get()->getPrototype();
            size_t  existingFunctionArgumentCount = ionIrPrototype->getArgs()->getItems().getSize();

            // Function already has a body, disallow re-definition.
            if (!ionIrFunction->get()->getBody()->getSymbolTable()->isEmpty()) {
                // TODO
                throw ionshared::Util::quickError<std::string>(
                    IONLANG_NOTICE_FUNCTION_ALREADY_DEFINED,
                    ionIrPrototype->getId()
                );
            }
            // If the function takes a different number of arguments, reject.
            else if (existingFunctionArgumentCount != argumentCount) {
                throw ionshared::Util::quickError(
                    IONLANG_NOTICE_FUNCTION_REDEFINITION_DIFFERENT_SIG,
                    ionIrPrototype->getId()
                );
            }
        }
        // Otherwise, function will be created.
        else {
            for (uint32_t i = 0; i < argumentCount; ++i) {
                // TODO: Process arguments.
//                arguments.push_back(llvm::Type::getDoubleTy(**this->contextBuffer));
            }

            // Visit and pop the return type.
            this->visitType(node->getReturnType());

            ionshared::Ptr<ionir::Type> ionIrReturnType = this->typeStack.pop();

            // TODO: Support for variable arguments and hard-coded return type.
            // TODO: Args and parent (ionir::Module) in form of IonIR entities.
            ionshared::Ptr<ionir::Prototype> ionIrPrototype =
                std::make_shared<ionir::Prototype>(node->getId(), nullptr, ionIrReturnType, nullptr);

            // TODO: Hard coded until figured.
            auto ionIrFunctionBody = 0;

            ionIrFunction = std::make_shared<ionir::Function>(ionIrPrototype, ionIrFunctionBody);

            /**
             * Insert the previously created function and register it
             * within the current module buffer.
             */
            this->moduleBuffer->get()->insertFunction(*ionIrFunction);
        }

        uint32_t i = 0;
        auto args = ionIrFunction->get()->getPrototype()->getArgs()->getItems().unwrap();

        for (const auto &arg : args) {
            // TODO: getItems() no longer a vector; cannot index by index, only key.
            // Retrieve the name element from the argument tuple.
            //            std::string name = node->getArgs()->getItems()[i].second;

            // Name the argument.
            //            arg.setName(name);

            // Increment the counter to prepare for next iteration.
            i++;
        }

        this->constructStack.push(*ionIrFunction);
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
}
