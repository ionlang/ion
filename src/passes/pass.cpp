#include <ionlang/passes/pass.h>

namespace ionlang {
    Pass::Pass(ionshared::Ptr<ionshared::PassContext> context) :
        ionshared::BasePass<Construct>(std::move(context)) {
        //
    }

    void Pass::visit(ionshared::Ptr<Construct> node) {
        // TODO: Hotfix for circular dep.
        if (node->constructKind == ConstructKind::Ref) {
            this->visitRef(node->staticCast<Ref<>>());
        }
        else if (node->constructKind == ConstructKind::Value) {
            this->visitValue(node->staticCast<Value<>>());
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

    void Pass::visitStatement(ionshared::Ptr<Statement> node) {
        switch (node->statementKind) {
            case StatementKind::If: {
                this->visitIfStatement(node->dynamicCast<IfStatement>());

                break;
            }

            case StatementKind::Return: {
                this->visitReturnStatement(node->dynamicCast<ReturnStatement>());

                break;
            }

            case StatementKind::VariableDeclaration: {
                this->visitVariableDecl(node->dynamicCast<VariableDeclStatement>());

                break;
            }

            case StatementKind::Assignment: {
                this->visitAssignmentStatement(node->dynamicCast<AssignmentStatement>());

                break;
            }

            case StatementKind::ExprWrapper: {
                this->visitExpression(node->dynamicCast<ExprWrapperStatement>()->getExpression());

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

    void Pass::visitExprWrapperStatement(ionshared::Ptr<ExprWrapperStatement> node) {
        //
    }

    void Pass::visitBlockWrapperStatement(ionshared::Ptr<BlockWrapperStatement> node) {
        //
    }

    void Pass::visitExpression(ionshared::Ptr<Expression> node) {
        switch (node->expressionKind) {
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

    void Pass::visitVariableRefExpr(ionshared::Ptr<VariableRefExpr> node) {
        //
    }

    void Pass::visitValue(ionshared::Ptr<Value<>> node) {
        switch (node->getValueKind()) {
            case ValueKind::Character: {
                this->visitCharLiteral(node->dynamicCast<CharLiteral>());

                break;
            }

            case ValueKind::Integer: {
                this->visitIntegerLiteral(node->dynamicCast<IntegerLiteral>());

                break;
            }

            case ValueKind::String: {
                this->visitStringLiteral(node->dynamicCast<StringLiteral>());

                break;
            }

            case ValueKind::Boolean: {
                this->visitBooleanLiteral(node->dynamicCast<BooleanLiteral>());

                break;
            }

            case ValueKind::Expression: {
                this->visitExpression(node->dynamicCast<Expression>());

                break;
            }

            default: {
                throw std::runtime_error("Unknown value kind");
            }
        }
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
