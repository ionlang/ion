#include <ionlang/construct/prototype.h>

namespace ionlang {
    Prototype::Prototype(std::string id, ionshared::Ptr<Args> args, ionshared::Ptr<Type> returnType, ionshared::Ptr<Module> parent)
        : ChildConstruct<Module>(parent, ConstructKind::Prototype), Named(id), args(args), returnType(returnType) {
        //
    }

    void Prototype::accept(Pass &visitor) {
        visitor.visitPrototype(this->dynamicCast<Prototype>());
    }

    ionshared::Ptr<Args> Prototype::getArgs() const noexcept {
        return this->args;
    }

    void Prototype::setArgs(ionshared::Ptr<Args> args) noexcept {
        this->args = args;
    }

    ionshared::Ptr<Type> Prototype::getReturnType() const noexcept {
        return this->returnType;
    }

    void Prototype::setReturnType(ionshared::Ptr<Type> returnType) noexcept {
        this->returnType = returnType;
    }
}
