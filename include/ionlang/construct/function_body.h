#pragma once

namespace ionlang {
    class Pass;

    class Function;

    class FunctionBody : public ChildConstruct<Function>, public ScopeAnchor<BasicBlock> {
    public:
        explicit FunctionBody(ionshared::Ptr<Function> parent, PtrSymbolTable<BasicBlock> symbolTable = TypeFactory::makePtrSymbolTable<BasicBlock>());

        void accept(Pass &visitor) override;

        Ast getChildNodes() override;

        bool verify() override;

        ionshared::OptPtr<BasicBlock> findEntryBasicBlock();

        bool hasEntryBasicBlock();

        void insertBasicBlock(ionshared::Ptr<BasicBlock> basicBlock);
    };
}
