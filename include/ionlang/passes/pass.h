#pragma once

#include <ionshared/misc/helpers.h>
#include <ionshared/passes/base_pass.h>
#include <ionshared/passes/base_pass_manager.h>
#include <ionlang/construct/pseudo/resolvable.h>
#include <ionlang/construct/pseudo/error_marker.h>
#include <ionlang/construct/construct.h>
#include <ionlang/construct/module.h>
#include <ionlang/construct/prototype.h>
#include <ionlang/construct/extern.h>
#include <ionlang/construct/block.h>
#include <ionlang/construct/statement.h>
#include <ionlang/construct/statement/if_statement.h>
#include <ionlang/construct/statement/return_statement.h>
#include <ionlang/construct/statement/variable_decl_statement.h>
#include <ionlang/construct/statement/assignment_statement.h>
#include <ionlang/construct/statement/expr_wrapper_statement.h>
#include <ionlang/construct/statement/block_wrapper_statement.h>
#include <ionlang/construct/expression.h>
#include <ionlang/construct/expression/operation.h>
#include <ionlang/construct/expression/call_expr.h>
#include <ionlang/construct/expression/variable_ref_expr.h>
#include <ionlang/construct/value/boolean_literal.h>
#include <ionlang/construct/value/char_literal.h>
#include <ionlang/construct/value/integer_literal.h>
#include <ionlang/construct/value/string_literal.h>
#include <ionlang/construct/type.h>
#include <ionlang/construct/type/void_type.h>
#include <ionlang/construct/type/boolean_type.h>
#include <ionlang/construct/type/user_defined_type.h>
#include <ionlang/construct/function.h>
#include <ionlang/construct/global.h>
#include <ionlang/construct/attribute.h>
#include <ionlang/construct/struct.h>
#include <ionlang/construct/struct_definition.h>

namespace ionlang {
    struct Pass : ionshared::BasePass<Construct> {
        explicit Pass(std::shared_ptr<ionshared::PassContext> context);

        virtual void visit(std::shared_ptr<Construct> node);

        virtual void visitChildren(std::shared_ptr<Construct> node);

        virtual void visitModule(std::shared_ptr<Module> node);

        virtual void visitPrototype(std::shared_ptr<Prototype> node);

        virtual void visitExtern(std::shared_ptr<Extern> node);

        virtual void visitBlock(std::shared_ptr<Block> node);

        virtual void visitIfStatement(std::shared_ptr<IfStmt> node);

        virtual void visitReturnStatement(std::shared_ptr<ReturnStmt> node);

        virtual void visitAssignmentStatement(std::shared_ptr<AssignmentStmt> node);

        virtual void visitExprWrapperStatement(std::shared_ptr<ExprWrapperStmt> node);

        virtual void visitBlockWrapperStatement(std::shared_ptr<BlockWrapperStmt> node);

        virtual void visitOperationExpr(std::shared_ptr<OperationExpr> node);

        virtual void visitCallExpr(std::shared_ptr<CallExpr> node);

        virtual void visitVariableRefExpr(std::shared_ptr<VariableRefExpr> node);

        virtual void visitIntegerLiteral(std::shared_ptr<IntegerLiteral> node);

        virtual void visitCharLiteral(std::shared_ptr<CharLiteral> node);

        virtual void visitStringLiteral(std::shared_ptr<StringLiteral> node);

        virtual void visitBooleanLiteral(std::shared_ptr<BooleanLiteral> node);

        virtual void visitVoidType(std::shared_ptr<VoidType> node);

        virtual void visitBooleanType(std::shared_ptr<BooleanType> node);

        virtual void visitUserDefinedType(std::shared_ptr<UserDefinedType> node);

        virtual void visitResolvable(PtrResolvable<> node);

        virtual void visitFunction(std::shared_ptr<Function> node);

        virtual void visitVariableDecl(std::shared_ptr<VariableDeclStmt> node);

        virtual void visitScopeAnchor(std::shared_ptr<ionshared::Scoped<Construct>> node);

        virtual void visitIntegerType(std::shared_ptr<IntegerType> node);

        virtual void visitGlobal(std::shared_ptr<Global> node);

        virtual void visitErrorMarker(std::shared_ptr<ErrorMarker> node);

        virtual void visitAttribute(std::shared_ptr<Attribute> node);

        virtual void visitStruct(std::shared_ptr<Struct> node);

        virtual void visitStructDefinition(std::shared_ptr<StructDefinition> construct);
    };

    typedef ionshared::BasePassManager<Pass, Construct> PassManager;
}
