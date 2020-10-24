#pragma once

#include <string>
#include <ionshared/misc/helpers.h>
#include <ionlang/construct/pseudo/resolvable.h>
#include <ionlang/construct/statement.h>
#include "variable_decl_statement.h"

namespace ionlang {
    struct Pass;

    struct AssignmentStmt : Statement {
        static std::shared_ptr<AssignmentStmt> make(
            const PtrResolvable<VariableDeclStmt>& variableDeclStatementRef,
            const std::shared_ptr<Expression<>>& value
        ) noexcept;

        PtrResolvable<VariableDeclStmt> variableDeclStmtRef;

        std::shared_ptr<Expression<>> value;

        explicit AssignmentStmt(
            PtrResolvable<VariableDeclStmt> variableDeclStmt,
            std::shared_ptr<Expression<>> value
        );

        void accept(Pass& visitor) override;

        [[nodiscard]] Ast getChildNodes() override;
    };
}
