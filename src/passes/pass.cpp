#include <ionlang/passes/pass.h>

namespace ionlang {
    Pass::Pass(std::shared_ptr<ionshared::PassContext> context) :
        ionshared::BasePass<Construct>(std::move(context)) {
        //
    }

    void Pass::visit(std::shared_ptr<Construct> node) {
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

    void Pass::visitChildren(std::shared_ptr<Construct> node) {
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

    void Pass::visitModule(std::shared_ptr<Module> node) {
        //
    }

    void Pass::visitPrototype(std::shared_ptr<Prototype> node) {
        //
    }

    void Pass::visitExtern(std::shared_ptr<Extern> node) {
        //
    }

    void Pass::visitBlock(std::shared_ptr<Block> node) {
        //
    }

    void Pass::visitIfStatement(std::shared_ptr<IfStatement> node) {
        //
    }

    void Pass::visitReturnStatement(std::shared_ptr<ReturnStatement> node) {
        //
    }

    void Pass::visitAssignmentStatement(std::shared_ptr<AssignmentStatement> node) {
        //
    }

    void Pass::visitExprWrapperStatement(std::shared_ptr<ExprWrapperStatement> node) {
        //
    }

    void Pass::visitBlockWrapperStatement(std::shared_ptr<BlockWrapperStatement> node) {
        //
    }

    void Pass::visitOperationExpr(std::shared_ptr<OperationExpr> node) {
        //
    }

    void Pass::visitCallExpr(std::shared_ptr<CallExpr> node) {
        //
    }

    void Pass::visitVariableRefExpr(std::shared_ptr<VariableRefExpr> node) {
        //
    }

    void Pass::visitBooleanLiteral(std::shared_ptr<BooleanLiteral> node) {
        //
    }

    void Pass::visitCharLiteral(std::shared_ptr<CharLiteral> node) {
        //
    }

    void Pass::visitIntegerLiteral(std::shared_ptr<IntegerLiteral> node) {
        //
    }

    void Pass::visitStringLiteral(std::shared_ptr<StringLiteral> node) {
        //
    }

    void Pass::visitVoidType(std::shared_ptr<VoidType> node) {
        //
    }

    void Pass::visitBooleanType(std::shared_ptr<BooleanType> node) {
        //
    }

    void Pass::visitUserDefinedType(std::shared_ptr<UserDefinedType> node) {
        //
    }

    void Pass::visitResolvable(PtrResolvable<> node) {
        //
    }

    void Pass::visitFunction(std::shared_ptr<Function> node) {
        //
    }

    void Pass::visitVariableDecl(std::shared_ptr<VariableDeclStatement> node) {
        //
    }

    void Pass::visitScopeAnchor(std::shared_ptr<ionshared::Scoped<Construct>> node) {
        //
    }

    void Pass::visitIntegerType(std::shared_ptr<IntegerType> node) {
        //
    }

    void Pass::visitGlobal(ionshared::Ptr <Global> node) {
        //
    }

    void Pass::visitErrorMarker(std::shared_ptr<ErrorMarker> node) {
        //
    }

    void Pass::visitAttribute(std::shared_ptr<Attribute> node) {
        //
    }

    void Pass::visitStruct(std::shared_ptr<Struct> node) {
        //
    }

    void Pass::visitStructDefinition(std::shared_ptr<StructDefinition> construct) {
        //
    }
}
