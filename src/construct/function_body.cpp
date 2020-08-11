#include <ionlang/construct/function_body.h>

namespace ionlang {
    FunctionBody::FunctionBody(ionshared::Ptr<Function> parent, ionshared::PtrSymbolTable<BasicBlock> symbolTable)
        : ChildConstruct(parent, ConstructKind::FunctionBody), ScopeAnchor<BasicBlock>(symbolTable) {
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

    bool FunctionBody::verify() {
        return this->hasEntryBasicBlock();
    }

    ionshared::OptPtr<BasicBlock> FunctionBody::findEntryBasicBlock() {
        return this->getSymbolTable()->lookup(Const::basicBlockEntryId);
    }

    bool FunctionBody::hasEntryBasicBlock() {
        return Util::hasValue(this->findEntryBasicBlock());
    }

    void FunctionBody::insertBasicBlock(ionshared::Ptr<BasicBlock> basicBlock) {
        // TODO: Check if section exists first?
        (*this->getSymbolTable())[basicBlock->getId()] = basicBlock;
    }
}
