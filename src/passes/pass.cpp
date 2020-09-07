#include <ionlang/passes/pass.h>

namespace ionlang {
    void Pass::visit(ionshared::Ptr<Construct> node) {
        node->accept(*this);

        // TODO: Hotfix for circular dep.
        if (node->getConstructKind() == ConstructKind::Ref) {
            this->visitRef(node->staticCast<Ref<>>());

            // No need to visit children for this node.
            return;
        }
        else if (node->getConstructKind() == ConstructKind::Value) {
            this->visitValue(node->staticCast<Value<>>());

            // No need to visit children for this node.
            return;
        }

        this->visitChildren(node);
    }

    void Pass::visitChildren(ionshared::Ptr<Construct> node) {
        // TODO: Will it cause StackOverflow error with large ASTs?
        /**
         * After visiting the node, attempt to
         * visit all its children as well.
         */
        for (const auto child : node->getChildNodes()) {
            // TODO: CRITICAL: What if 'child' (AstNode) is not boxed under Construct?
            this->visit(child->staticCast<Construct>());
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

    void Pass::visitStatement(ionshared::Ptr<Statement> node) {
        switch (node->getStatementKind()) {
            case StatementKind::If: {
                this->visitIfStatement(node->dynamicCast<IfStatement>());

                break;
            }

            case StatementKind::Return: {
                this->visitReturnStatement(node->dynamicCast<ReturnStatement>());

                break;
            }

            case StatementKind::VariableDeclaration: {
                this->visitVariableDecl(node->dynamicCast<VariableDecl>());

                break;
            }

            // TODO: Add for all statements. Critical!

            default: {
                throw std::runtime_error("Unknown statement kind");
            }
        }
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

    void Pass::visitExpression(ionshared::Ptr<Expression> node) {
        switch (node->getExpressionKind()) {
            case ExpressionKind::UnaryOperation: {
                this->visitUnaryOperation(node->dynamicCast<UnaryOperation>());

                break;
            }

            case ExpressionKind::BinaryOperation: {
                this->visitBinaryOperation(node->dynamicCast<BinaryOperation>());

                break;
            }

            case ExpressionKind::Call: {
                this->visitCallExpr(node->dynamicCast<CallExpr>());

                break;
            }

            // TODO: Add support for missing expression(s)...

            default: {
                throw std::runtime_error("Unknown expression kind");
            }
        }
    }

    void Pass::visitUnaryOperation(ionshared::Ptr<UnaryOperation> node) {
        //
    }

    void Pass::visitBinaryOperation(ionshared::Ptr<BinaryOperation> node) {
        //
    }

    void Pass::visitCallExpr(ionshared::Ptr<CallExpr> node) {
        //
    }

    void Pass::visitValue(ionshared::Ptr<Value<>> node) {
        switch (node->getValueKind()) {
            case ValueKind::Character: {
                this->visitCharValue(node->dynamicCast<CharLiteral>());

                break;
            }

            case ValueKind::Integer: {
                this->visitIntegerValue(node->dynamicCast<IntegerLiteral>());

                break;
            }

            case ValueKind::String: {
                this->visitStringValue(node->dynamicCast<StringLiteral>());

                break;
            }

            case ValueKind::Boolean: {
                this->visitBooleanValue(node->dynamicCast<BooleanLiteral>());

                break;
            }

            default: {
                throw std::runtime_error("Unknown value kind");
            }
        }
    }

    void Pass::visitBooleanValue(ionshared::Ptr<BooleanLiteral> node) {
        //
    }

    void Pass::visitCharValue(ionshared::Ptr<CharLiteral> node) {
        //
    }

    void Pass::visitIntegerValue(ionshared::Ptr<IntegerLiteral> node) {
        //
    }

    void Pass::visitStringValue(ionshared::Ptr<StringLiteral> node) {
        //
    }

    void Pass::visitType(ionshared::Ptr<Type> node) {
        //
    }

    void Pass::visitVoidType(ionshared::Ptr<VoidType> node) {
        //
    }

    void Pass::visitBooleanType(ionshared::Ptr<BooleanType> node) {
        //
    }

    void Pass::visitRef(PtrRef<> node) {
        //
    }

    void Pass::visitFunction(ionshared::Ptr<Function> node) {
        //
    }

    void Pass::visitVariableDecl(ionshared::Ptr<VariableDecl> node) {
        //
    }

    void Pass::visitScopeAnchor(ionshared::Ptr<ionshared::ScopeAnchor<Construct>> node) {
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
}
