#pragma once

#include <ionshared/misc/helpers.h>
#include <ionshared/passes/base_pass.h>
#include <ionshared/passes/base_pass_manager.h>
#include <ionlang/construct/pseudo/ref.h>
#include <ionlang/construct/pseudo/error_marker.h>
#include <ionlang/construct/construct.h>
#include <ionlang/construct/module.h>
#include <ionlang/construct/prototype.h>
#include <ionlang/construct/extern.h>
#include <ionlang/construct/block.h>
#include <ionlang/construct/statement.h>
#include <ionlang/construct/statement/if_statement.h>
#include <ionlang/construct/statement/return_statement.h>
#include <ionlang/construct/statement/variable_declaration.h>
#include <ionlang/construct/statement/assignment_statement.h>
#include <ionlang/construct/expression.h>
#include <ionlang/construct/expression/binary_operation.h>
#include <ionlang/construct/expression/call_expr.h>
#include <ionlang/construct/value.h>
#include <ionlang/construct/value/boolean_literal.h>
#include <ionlang/construct/value/char_literal.h>
#include <ionlang/construct/value/integer_literal.h>
#include <ionlang/construct/value/string_literal.h>
#include <ionlang/construct/type.h>
#include <ionlang/construct/type/void_type.h>
#include <ionlang/construct/type/boolean_type.h>
#include <ionlang/construct/function.h>
#include <ionlang/construct/global.h>
#include <ionlang/construct/attribute.h>

namespace ionlang {
    class Pass : public ionshared::BasePass<Construct> {
    public:
        virtual void visit(ionshared::Ptr<Construct> node);

        virtual void visitChildren(ionshared::Ptr<Construct> node);

        virtual void visitModule(ionshared::Ptr<Module> node);

        virtual void visitPrototype(ionshared::Ptr<Prototype> node);

        virtual void visitExtern(ionshared::Ptr<Extern> node);

        virtual void visitBlock(ionshared::Ptr<Block> node);

        virtual void visitStatement(ionshared::Ptr<Statement> node);

        virtual void visitIfStatement(ionshared::Ptr<IfStatement> node);

        virtual void visitReturnStatement(ionshared::Ptr<ReturnStatement> node);

        virtual void visitAssignmentStatement(ionshared::Ptr<AssignmentStatement> node);

        virtual void visitExpression(ionshared::Ptr<Expression> node);

        virtual void visitUnaryOperation(ionshared::Ptr<UnaryOperation> node);

        virtual void visitBinaryOperation(ionshared::Ptr<BinaryOperation> node);

        virtual void visitCallExpr(ionshared::Ptr<CallExpr> node);

        virtual void visitValue(ionshared::Ptr<Value<>> node);

        virtual void visitIntegerValue(ionshared::Ptr<IntegerLiteral> node);

        virtual void visitCharValue(ionshared::Ptr<CharLiteral> node);

        virtual void visitStringValue(ionshared::Ptr<StringLiteral> node);

        virtual void visitBooleanValue(ionshared::Ptr<BooleanLiteral> node);

        virtual void visitType(ionshared::Ptr<Type> node);

        virtual void visitVoidType(ionshared::Ptr<VoidType> node);

        virtual void visitBooleanType(ionshared::Ptr<BooleanType> node);

        virtual void visitRef(PtrRef<> node);

        virtual void visitFunction(ionshared::Ptr<Function> node);

        virtual void visitVariableDecl(ionshared::Ptr<VariableDecl> node);

        virtual void visitScopeAnchor(ionshared::Ptr<ionshared::ScopeAnchor<Construct>> node);

        virtual void visitIntegerType(ionshared::Ptr<IntegerType> node);

        virtual void visitGlobal(ionshared::Ptr<Global> node);

        virtual void visitErrorMarker(ionshared::Ptr<ErrorMarker> node);

        virtual void visitAttribute(ionshared::Ptr<Attribute> node);
    };

    typedef ionshared::BasePassManager<Pass, Construct> PassManager;

    typedef ionshared::PassManagerItem<Pass> PassManagerItem;
}
