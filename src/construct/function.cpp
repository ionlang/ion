#include <ionlang/passes/pass.h>

namespace ionlang {
    Function::Function(ionshared::Ptr<Prototype> prototype, ionshared::Ptr<FunctionBody> body)
        : Construct(ConstructKind::Function), prototype(prototype), body(body) {
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
        this->prototype = prototype;
    }

    ionshared::Ptr<FunctionBody> Function::getBody() const noexcept {
        return this->body;
    }

    void Function::setBody(ionshared::Ptr<FunctionBody> body) noexcept {
        this->body = body;
    }

    ionshared::PtrSymbolTable<ionshared::LocalVariableDescriptor> Function::getLocalVariables() const {
        return this->localVariables;
    }

    void Function::setLocalVariables(ionshared::PtrSymbolTable<ionshared::LocalVariableDescriptor> localVariables) {
        this->localVariables = localVariables;
    }
}
