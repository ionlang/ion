#pragma once

#include <ionshared/misc/helpers.h>
#include <ionlang/construct/pseudo/child_construct.h>

namespace ionlang {
    struct Pass;

    struct Block;

    enum struct StatementKind {
        If,

        Return,

        VariableDeclaration,

        Assignment,

        Call,

        ExprWrapper,

        BlockWrapper
    };

    struct Statement : ConstructWithParent<Block> {
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
