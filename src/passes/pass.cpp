#include <ionlang/passes/pass.h>

namespace ionlang {
    Pass::Pass(ionshared::Ptr<ionshared::PassContext> context) :
        ionshared::BasePass<Construct>(std::move(context)) {
        //
    }

    void Pass::visit(ionshared::Ptr<Construct> node) {
        // TODO: Hotfix for circular dep.
        if (node->constructKind == ConstructKind::Resolvable) {
            this->visitResolvable(node->staticCast<Resolvable<>>());
        }
        // TODO: This should be at the top alone with visitChildren() once fixed.
        else {
            node->accept(*this);
        }

        this->visitChildren(node);
    }

    void Pass::visitChildren(ionshared::Ptr<Construct> node) {
        // TODO: Will it cause StackOverflow error with large ASTs?
        Ast children = node->getChildNodes();

        /**
         * After visiting the node, attempt to visit all its children
         * as well.
         */
        for (const auto &child : children) {
            this->visit(child);
        }
    }

    void Pass::visitModule(ionshared::Ptr<Module> node) {
        //
    }

    void Pass::visitPrototype(ionshared::Ptr<Prototype> node) {
        //
    }

    void Pass::visitExtern(ionshared::Ptr<Extern> node) {
        //
    }

    void Pass::visitBlock(ionshared::Ptr<Block> node) {
        //
    }

    void Pass::visitIfStatement(ionshared::Ptr<IfStatement> node) {
        //
    }

    void Pass::visitReturnStatement(ionshared::Ptr<ReturnStatement> node) {
        //
    }

    void Pass::visitAssignmentStatement(ionshared::Ptr<AssignmentStatement> node) {
        //
    }

    void Pass::visitExprWrapperStatement(ionshared::Ptr<ExprWrapperStatement> node) {
        //
    }

    void Pass::visitBlockWrapperStatement(ionshared::Ptr<BlockWrapperStatement> node) {
        //
    }

    void Pass::visitOperationExpr(ionshared::Ptr<OperationExpr> node) {
        //
    }

    void Pass::visitCallExpr(ionshared::Ptr<CallExpr> node) {
        //
    }

    void Pass::visitVariableRefExpr(ionshared::Ptr<VariableRefExpr> node) {
        //
    }

    void Pass::visitBooleanLiteral(ionshared::Ptr<BooleanLiteral> node) {
        //
    }

    void Pass::visitCharLiteral(ionshared::Ptr<CharLiteral> node) {
        //
    }

    void Pass::visitIntegerLiteral(ionshared::Ptr<IntegerLiteral> node) {
        //
    }

    void Pass::visitStringLiteral(ionshared::Ptr<StringLiteral> node) {
        //
    }

    void Pass::visitVoidType(ionshared::Ptr<VoidType> node) {
        //
    }

    void Pass::visitBooleanType(ionshared::Ptr<BooleanType> node) {
        //
    }

    void Pass::visitUserDefinedType(ionshared::Ptr<UserDefinedType> node) {
        //
    }

    void Pass::visitResolvable(PtrResolvable<> node) {
        //
    }

    void Pass::visitFunction(ionshared::Ptr<Function> node) {
        //
    }

    void Pass::visitVariableDecl(ionshared::Ptr<VariableDeclStatement> node) {
        //
    }

    void Pass::visitScopeAnchor(ionshared::Ptr<ionshared::Scoped<Construct>> node) {
        //
    }

    void Pass::visitIntegerType(ionshared::Ptr<IntegerType> node) {
        //
    }

    void Pass::visitGlobal(ionshared::Ptr <Global> node) {
        //
    }

    void Pass::visitErrorMarker(ionshared::Ptr<ErrorMarker> node) {
        //
    }

    void Pass::visitAttribute(ionshared::Ptr<Attribute> node) {
        //
    }

    void Pass::visitStruct(ionshared::Ptr<Struct> node) {
        //
    }
}
