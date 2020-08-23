#include <ionlang/passes/pass.h>

namespace ionlang {
    Function::Function(ionshared::Ptr<Prototype> prototype, ionshared::Ptr<Block> body)
        : Construct(ConstructKind::Function), prototype(std::move(prototype)), body(std::move(body)) {
        //
    }

    void Function::accept(Pass &visitor) {
        visitor.visitFunction(this->dynamicCast<Function>());
    }

    Ast Function::getChildNodes() {
        return {
            this->prototype->nativeCast(),
            this->body->nativeCast()
        };
    }

    ionshared::Ptr<Prototype> Function::getPrototype() const noexcept {
        return this->prototype;
    }

    void Function::setPrototype(ionshared::Ptr<Prototype> prototype) {
        this->prototype = std::move(prototype);
    }

    ionshared::Ptr<Block> Function::getBody() const noexcept {
        return this->body;
    }

    void Function::setBody(ionshared::Ptr<Block> body) noexcept {
        this->body = std::move(body);
    }

    ionshared::PtrSymbolTable<LocalVariableDescriptor> Function::getLocalVariables() const {
        return this->localVariables;
    }

    void Function::setLocalVariables(ionshared::PtrSymbolTable<LocalVariableDescriptor> localVariables) {
        this->localVariables = std::move(localVariables);
    }
}
