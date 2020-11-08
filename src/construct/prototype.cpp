#include <ionlang/passes/pass.h>

#define IONLANG_MANGLE_SEPARATOR "_"

namespace ionlang {
    std::shared_ptr<Prototype> Prototype::make(
        const std::string& name,
        const std::shared_ptr<ArgumentList>& argumentList,
        const PtrResolvable<Type>& returnType
    ) noexcept {
        std::shared_ptr<Prototype> result =
            std::make_shared<Prototype>(name, argumentList, returnType);

        argumentList->parent = result;
        returnType->parent = result;

        return result;
    }

    Prototype::Prototype(
        std::string name,
        std::shared_ptr<ArgumentList> argumentList,
        PtrResolvable<Type> returnType
    ) :
        Construct(ConstructKind::Prototype),
        Named{std::move(name)},
        argumentList(std::move(argumentList)),
        returnType(std::move(returnType)) {
        //
    }

    void Prototype::accept(Pass& visitor) {
        visitor.visitPrototype(this->dynamicCast<Prototype>());
    }

    Ast Prototype::getChildNodes() {
        return {
            this->argumentList,
            this->returnType
        };
    }

    std::optional<std::string> Prototype::getMangledName() {
        std::shared_ptr<Construct> localParent = this->forceGetParent();
        ConstructKind parentConstructKind = localParent->constructKind;

        if (parentConstructKind != ConstructKind::Extern
            && parentConstructKind != ConstructKind::Function) {
            return std::nullopt;
        }

        std::shared_ptr<ConstructWithParent<Module>> localParentAsChild =
            localParent->dynamicCast<ConstructWithParent<Module>>();

        // TODO: Need to make sure that mangled id is compatible with LLVM IR ids.
        std::stringstream mangledName{};

        mangledName << localParentAsChild->forceGetUnboxedParent()->name
            << IONLANG_MANGLE_SEPARATOR
            << this->returnType->id.value_or(this->returnType->forceGetValue()->typeName)
            << IONLANG_MANGLE_SEPARATOR
            << this->name;

        auto argumentListNativeMap = this->argumentList->symbolTable->unwrap();

        for (const auto& [name, type] : argumentListNativeMap) {
            mangledName << IONLANG_MANGLE_SEPARATOR
                << type->id.value_or(type->forceGetValue()->typeName)
                << IONLANG_MANGLE_SEPARATOR
                << name;
        }

        return mangledName.str();
    }
}
