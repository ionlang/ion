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
#include <ionlang/construct/type/struct_type.h>
#include <ionlang/construct/function.h>
#include <ionlang/construct/global.h>
#include <ionlang/construct/attribute.h>
#include <ionlang/construct/struct_definition.h>
#include <ionlang/construct/argument_list.h>
#include <ionlang/construct/identifier.h>
#include <ionlang/construct/import.h>
#include <ionlang/construct/cast.h>

namespace ionlang {
    struct Pass : ionshared::BasePass<Construct> {
        explicit Pass(std::shared_ptr<ionshared::PassContext> context);

        void visit(std::shared_ptr<Construct> construct) override;

        virtual void visitChildren(std::shared_ptr<Construct> construct);

        virtual void visitModule(std::shared_ptr<Module> construct);

        virtual void visitPrototype(std::shared_ptr<Prototype> construct);

        virtual void visitExtern(std::shared_ptr<Extern> construct);

        virtual void visitBlock(std::shared_ptr<Block> construct);

        virtual void visitIfStmt(std::shared_ptr<IfStmt> construct);

        virtual void visitReturnStmt(std::shared_ptr<ReturnStmt> construct);

        virtual void visitAssignmentStmt(std::shared_ptr<AssignmentStmt> construct);

        virtual void visitExprWrapperStmt(std::shared_ptr<ExprWrapperStmt> construct);

        virtual void visitBlockWrapperStatement(std::shared_ptr<BlockWrapperStmt> construct);

        virtual void visitOperationExpr(std::shared_ptr<OperationExpr> construct);

        virtual void visitCallExpr(std::shared_ptr<CallExpr> construct);

        virtual void visitVariableRefExpr(std::shared_ptr<VariableRefExpr> construct);

        virtual void visitIntegerLiteral(std::shared_ptr<IntegerLiteral> construct);

        virtual void visitCharLiteral(std::shared_ptr<CharLiteral> construct);

        virtual void visitStringLiteral(std::shared_ptr<StringLiteral> construct);

        virtual void visitBooleanLiteral(std::shared_ptr<BooleanLiteral> construct);

        virtual void visitVoidType(std::shared_ptr<VoidType> construct);

        virtual void visitBooleanType(std::shared_ptr<BooleanType> construct);

        virtual void visitIntegerType(std::shared_ptr<IntegerType> construct);

        virtual void visitStructType(std::shared_ptr<StructType> construct);

        virtual void visitResolvable(PtrResolvable<> construct);

        virtual void visitFunction(std::shared_ptr<Function> construct);

        virtual void visitVariableDeclStmt(std::shared_ptr<VariableDeclStmt> construct);

        virtual void visitScopeAnchor(std::shared_ptr<Scoped<>> construct);

        virtual void visitGlobal(std::shared_ptr<Global> construct);

        virtual void visitErrorMarker(std::shared_ptr<ErrorMarker> construct);

        virtual void visitAttribute(std::shared_ptr<Attribute> construct);

        virtual void visitStructDefinition(std::shared_ptr<StructDefinition> construct);

        virtual void visitArgumentList(std::shared_ptr<ArgumentList> construct);

        virtual void visitIdentifier(std::shared_ptr<Identifier> construct);

        virtual void visitImport(std::shared_ptr<Import> construct);

        virtual void visitCast(std::shared_ptr<Cast> construct);
    };

    typedef ionshared::BasePassManager<Pass, Construct> PassManager;
}
