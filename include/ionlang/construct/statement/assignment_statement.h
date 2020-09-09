#pragma once

#include <string>
#include <ionshared/misc/helpers.h>
#include <ionlang/construct/pseudo/ref.h>
#include <ionlang/construct/value.h>
#include <ionlang/construct/statement.h>
#include "variable_decl_statement.h"

namespace ionlang {
    class Pass;

    struct AssignmentStatementOpts : StatementOpts {
        PtrRef<VariableDeclStatement> variableDeclStatementRef;

        ionshared::Ptr<Construct> value;
    };

    class AssignmentStatement : public Statement {
    private:
        PtrRef<VariableDeclStatement> variableDeclStatementRef;

        ionshared::Ptr<Construct> value;

    public:
        explicit AssignmentStatement(const AssignmentStatementOpts &opts);

        void accept(Pass &visitor) override;

        Ast getChildNodes() override;

        [[nodiscard]] PtrRef<VariableDeclStatement> getVariableDeclStatement() const noexcept;

        void setVariableDeclStatement(PtrRef<VariableDeclStatement> variableDeclStatement) noexcept;

        ionshared::Ptr<Construct> getValue() const noexcept;

        void setValue(ionshared::Ptr<Construct> value) noexcept;
    };
}
