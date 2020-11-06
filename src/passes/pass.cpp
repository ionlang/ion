#include <ionlang/passes/pass.h>

namespace ionlang {
    Pass::Pass(std::shared_ptr<ionshared::PassContext> context) :
        ionshared::BasePass<Construct>(std::move(context)) {
        //
    }

    void Pass::visit(std::shared_ptr<Construct> construct) {
        // TODO: Hotfix for circular dep.
        if (construct->constructKind == ConstructKind::Resolvable) {
            this->visitResolvable(construct->staticCast<Resolvable<>>());
        }
        // TODO: This should be at the top alone with visitChildren() once fixed.
        else {
            construct->accept(*this);
        }

        this->visitChildren(construct);
    }

    void Pass::visitChildren(std::shared_ptr<Construct> construct) {
        // TODO: Will it cause StackOverflow error with large ASTs?
        Ast children = construct->getChildNodes();

        /**
         * After visiting the construct, attempt to visit all its children
         * as well.
         */
        for (const auto& child : children) {
            this->visit(child);
        }
    }

    void Pass::visitModule(std::shared_ptr<Module> construct) {
        //
    }

    void Pass::visitPrototype(std::shared_ptr<Prototype> construct) {
        //
    }

    void Pass::visitExtern(std::shared_ptr<Extern> construct) {
        //
    }

    void Pass::visitBlock(std::shared_ptr<Block> construct) {
        //
    }

    void Pass::visitIfStmt(std::shared_ptr<IfStmt> construct) {
        //
    }

    void Pass::visitReturnStmt(std::shared_ptr<ReturnStmt> construct) {
        //
    }

    void Pass::visitAssignmentStmt(std::shared_ptr<AssignmentStmt> construct) {
        //
    }

    void Pass::visitExprWrapperStmt(std::shared_ptr<ExprWrapperStmt> construct) {
        //
    }

    void Pass::visitBlockWrapperStatement(std::shared_ptr<BlockWrapperStmt> construct) {
        //
    }

    void Pass::visitOperationExpr(std::shared_ptr<OperationExpr> construct) {
        //
    }

    void Pass::visitCallExpr(std::shared_ptr<CallExpr> construct) {
        //
    }

    void Pass::visitVariableRefExpr(std::shared_ptr<VariableRefExpr> construct) {
        //
    }

    void Pass::visitBooleanLiteral(std::shared_ptr<BooleanLiteral> construct) {
        //
    }

    void Pass::visitCharLiteral(std::shared_ptr<CharLiteral> construct) {
        //
    }

    void Pass::visitIntegerLiteral(std::shared_ptr<IntegerLiteral> construct) {
        //
    }

    void Pass::visitStringLiteral(std::shared_ptr<StringLiteral> construct) {
        //
    }

    void Pass::visitVoidType(std::shared_ptr<VoidType> construct) {
        //
    }

    void Pass::visitBooleanType(std::shared_ptr<BooleanType> construct) {
        //
    }

    void Pass::visitIntegerType(std::shared_ptr<IntegerType> construct) {
        //
    }

    void Pass::visitStructType(std::shared_ptr<StructType> construct) {
        //
    }

    void Pass::visitResolvable(PtrResolvable<> construct) {
        //
    }

    void Pass::visitFunction(std::shared_ptr<Function> construct) {
        //
    }

    void Pass::visitVariableDeclStmt(std::shared_ptr<VariableDeclStmt> construct) {
        //
    }

    void Pass::visitScopeAnchor(std::shared_ptr<Scoped<>> construct) {
        //
    }

    void Pass::visitGlobal(ionshared::Ptr <Global> construct) {
        //
    }

    void Pass::visitErrorMarker(std::shared_ptr<ErrorMarker> construct) {
        //
    }

    void Pass::visitAttribute(std::shared_ptr<Attribute> construct) {
        //
    }

    void Pass::visitStructDefinition(std::shared_ptr<StructDefinition> construct) {
        //
    }

    void Pass::visitArgumentList(std::shared_ptr<ArgumentList> construct) {
        //
    }

    void Pass::visitIdentifier(std::shared_ptr<Identifier> construct) {
        //
    }

    void Pass::visitImport(std::shared_ptr<Import> construct) {
        //
    }
}
