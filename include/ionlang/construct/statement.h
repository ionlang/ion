#pragma once

#include <ionshared/misc/helpers.h>
#include <ionlang/construct/pseudo/construct_with_parent.h>
#include "block.h"

namespace ionlang {
    struct Pass;

    enum struct StatementKind : uint32_t {
        If,

        Return,

        VariableDeclaration,

        Assignment,

        Call,

        ExprWrapper,

        BlockWrapper
    };

    struct Statement : ConstructWithParent<Block, Construct, ConstructKind> {
        const StatementKind statementKind;

        const ionshared::OptPtr<Statement> yields;

        explicit Statement(
            StatementKind kind,
            ionshared::OptPtr<Statement> yields = std::nullopt
        );

        void accept(Pass& visitor) override = 0;

        [[nodiscard]] bool isTerminal() const noexcept;

        [[nodiscard]] uint32_t getOrder();
    };
}
