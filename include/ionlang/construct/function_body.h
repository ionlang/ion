#pragma once

#include <ionshared/tracking/scope_anchor.h>
#include <ionshared/tracking/symbol_table.h>
#include "basic_block.h"
#include "function.h"

namespace ionlang {
    class Pass;

    class Function;

    class FunctionBody : public ChildConstruct<Function>, public ionshared::ScopeAnchor<BasicBlock> {
    public:
        explicit FunctionBody(
            ionshared::Ptr<Function> parent,
            ionshared::PtrSymbolTable<BasicBlock> symbolTable =
                ionshared::Util::makePtrSymbolTable<BasicBlock>()
        );

        void accept(Pass &visitor) override;

        Ast getChildNodes() override;

        ionshared::OptPtr<BasicBlock> findEntryBasicBlock();

        bool hasEntryBasicBlock();

        void insertBasicBlock(const ionshared::Ptr<BasicBlock> &basicBlock);
    };
}
