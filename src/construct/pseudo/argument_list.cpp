#include <ionlang/passes/pass.h>

namespace ionlang {
    std::shared_ptr<ArgumentList> ArgumentList::make(
        const ionshared::PtrSymbolTable<Construct>& symbolTable,
        bool isVariable
    ) noexcept {
        std::shared_ptr<ArgumentList> result =
            std::make_shared<ArgumentList>(symbolTable, isVariable);

        auto itemsNativeMap = symbolTable->unwrap();

        for (const auto& [name, construct] : itemsNativeMap) {
            construct->setParent(result);
        }

        return result;
    }

    ArgumentList::ArgumentList(
        const ionshared::PtrSymbolTable<Construct>& symbolTable,
        bool isVariable
    ) :
        Construct(ConstructKind::ArgumentList),
        ionshared::Scoped<Construct, ConstructKind>(symbolTable),
        isVariable(isVariable) {
        //
    }

    void ArgumentList::accept(Pass& visitor) {
        visitor.visitArgumentList(this->dynamicCast<ArgumentList>());
    }

    Ast ArgumentList::getChildNodes() {
        Ast children{};
        auto symbolTableNativeMap = this->symbolTable->unwrap();

        for (const auto& [name, type] : symbolTableNativeMap) {
            children.push_back(type);
        }

        return children;
    }
}
