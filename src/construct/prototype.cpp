#define IONLANG_MANGLE_SEPARATOR "_"

#include <ionlang/passes/pass.h>

namespace ionlang {
    Prototype::Prototype(
        std::string id,
        ionshared::Ptr<Args> args,
        ionshared::Ptr<Type> returnType,
        ionshared::Ptr<Construct> parent
    ) :
        ConstructWithParent<>(std::move(parent), ConstructKind::Prototype),
        Named{std::move(id)},
        args(std::move(args)),
        returnType(std::move(returnType)) {
        //
    }

    void Prototype::accept(Pass &visitor) {
        visitor.visitPrototype(this->dynamicCast<Prototype>());
    }

    std::optional<std::string> Prototype::getMangledId() {
        ionshared::Ptr<Construct> localParent = this->getUnboxedParent();
        ConstructKind parentConstructKind = localParent->constructKind;

        if (parentConstructKind != ConstructKind::Extern && parentConstructKind != ConstructKind::Function) {
            return std::nullopt;
        }

        ionshared::Ptr<ConstructWithParent<Module>> localParentAsChild =
            localParent->dynamicCast<ConstructWithParent<Module>>();

        // TODO: Need to make sure that mangled id is compatible with LLVM IR ids.
        std::stringstream mangledId;

        mangledId << localParentAsChild->getUnboxedParent()->name
            << IONLANG_MANGLE_SEPARATOR
            << this->returnType->name
            << IONLANG_MANGLE_SEPARATOR
            << this->name;

        auto argsMap = this->args->items->unwrap();

        for (const auto &[id, arg] : argsMap) {
            mangledId << IONLANG_MANGLE_SEPARATOR
                << arg.first->name
                << IONLANG_MANGLE_SEPARATOR
                << arg.second;
        }

        return mangledId.str();
    }
}
