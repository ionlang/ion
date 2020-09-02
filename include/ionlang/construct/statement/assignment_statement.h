#pragma once

#include <string>
#include <ionshared/misc/helpers.h>
#include <ionlang/construct/pseudo/ref.h>
#include <ionlang/construct/value.h>
#include <ionlang/construct/statement.h>
#include "variable_declaration.h"

namespace ionlang {
    class Pass;

    // TODO: Use StatementOpts struct for constructor options?

    class AssignmentStatement : public Statement {
    private:
        PtrRef<VariableDecl> variableDecl;

        ionshared::Ptr<Construct> value;

    public:
        AssignmentStatement(
            ionshared::Ptr<Block> parent,
            PtrRef<VariableDecl> variableDecl,
            ionshared::Ptr<Construct> value
        );

        void accept(Pass &visitor) override;

        [[nodiscard]] PtrRef<VariableDecl> getVariableDecl() const noexcept;

        void setVariableDecl(PtrRef<VariableDecl> variableDecl) noexcept;

        ionshared::Ptr<Construct> getValue() const noexcept;

        void setValue(ionshared::Ptr<Construct> value) noexcept;
    };
}
