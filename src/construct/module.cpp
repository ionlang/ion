#include <ionlang/passes/pass.h>

namespace ionlang {
    Module::Module(std::string id, ionshared::PtrSymbolTable<Construct> symbolTable)
        : Construct(ConstructKind::Module), ionshared::ScopeAnchor<Construct>(std::move(symbolTable)), ionshared::Named(std::move(id)) {
        //
    }

    void Module::accept(Pass &visitor) {
        visitor.visitModule(this->dynamicCast<Module>());
    }
}
