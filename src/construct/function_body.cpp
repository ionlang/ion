#include <utility>
#include <ionlang/const/const.h>
#include <ionlang/passes/pass.h>

namespace ionlang {
    FunctionBody::FunctionBody(ionshared::Ptr<Function> parent, ionshared::PtrSymbolTable<BasicBlock> symbolTable)
        : ChildConstruct(std::move(parent), ConstructKind::FunctionBody), ScopeAnchor<BasicBlock>(std::move(symbolTable)) {
        //
    }

    void FunctionBody::accept(Pass &visitor) {
        // TODO: CRITICAL: Cast error.
        //        visitor.visitScopeAnchor(this->staticCast<ScopeAnchor<Section>>());
        visitor.visitFunctionBody(this->staticCast<FunctionBody>());
    }

    Ast FunctionBody::getChildNodes() {
        // TODO: De-referencing symbol table, so it's copying and it won't link back? Review.
        return Construct::convertChildren(*this->getSymbolTable());
    }

    ionshared::OptPtr<BasicBlock> FunctionBody::findEntryBasicBlock() {
        return this->getSymbolTable()->lookup(Const::basicBlockEntryId);
    }

    bool FunctionBody::hasEntryBasicBlock() {
        return ionshared::Util::hasValue(this->findEntryBasicBlock());
    }

    void FunctionBody::insertBasicBlock(const ionshared::Ptr<BasicBlock> &basicBlock) {
        // TODO: Check if section exists first?
        (*this->getSymbolTable())[basicBlock->getId()] = basicBlock;
    }
}
