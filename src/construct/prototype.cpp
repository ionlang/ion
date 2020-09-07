#define IONLANG_MANGLE_SEPARATOR "_"

#include <ionlang/passes/pass.h>

namespace ionlang {
    Prototype::Prototype(
        std::string id,
        ionshared::Ptr<Args> args,
        ionshared::Ptr<Type> returnType,
        ionshared::Ptr<Construct> parent
    ) :
        ChildConstruct<>(std::move(parent), ConstructKind::Prototype),
        Named(std::move(id)),
        args(std::move(args)),
        returnType(std::move(returnType)) {
        //
    }

    void Prototype::accept(Pass &visitor) {
        visitor.visitPrototype(this->dynamicCast<Prototype>());
    }

    ionshared::Ptr<Args> Prototype::getArguments() const noexcept {
        return this->args;
    }

    void Prototype::setArgs(ionshared::Ptr<Args> args) noexcept {
        this->args = std::move(args);
    }

    ionshared::Ptr<Type> Prototype::getReturnType() const noexcept {
        return this->returnType;
    }

    void Prototype::setReturnType(ionshared::Ptr<Type> returnType) noexcept {
        this->returnType = std::move(returnType);
    }

    std::optional<std::string> Prototype::getMangledId() {
        ionshared::Ptr<Construct> localParent = this->getParent();
        ConstructKind parentConstructKind = localParent->getConstructKind();

        if (parentConstructKind != ConstructKind::Extern && parentConstructKind != ConstructKind::Function) {
            return std::nullopt;
        }

        ionshared::Ptr<ChildConstruct<Module>> localParentAsChild =
            localParent->dynamicCast<ChildConstruct<Module>>();

        // TODO: Need to make sure that mangled id is compatible with LLVM IR ids.
        std::stringstream mangledId;

        mangledId << localParentAsChild->getParent()->getId() << IONLANG_MANGLE_SEPARATOR
            << this->returnType->getId() << IONLANG_MANGLE_SEPARATOR
            << this->getId();

        auto argsMap = this->args->getItems()->unwrap();

        for (const auto &[id, arg] : argsMap) {
            mangledId << IONLANG_MANGLE_SEPARATOR
                << arg.first->getId() << IONLANG_MANGLE_SEPARATOR
                << arg.second;
        }

        return mangledId.str();
    }
}
