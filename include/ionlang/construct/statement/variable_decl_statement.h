#pragma once

#include <string>
#include <ionshared/misc/helpers.h>
#include <ionlang/construct/expression.h>
#include "ionlang/construct/statement.h"

namespace ionlang {
    struct Pass;

    struct VariableDeclStmt : Statement, ionshared::Named {
        static std::shared_ptr<VariableDeclStmt> make(
            const PtrResolvable<Type>& type,
            const std::string& id,
            const std::shared_ptr<Expression<>>& value
        ) noexcept;

        PtrResolvable<Type> type;

        std::shared_ptr<Expression<>> value;

        explicit VariableDeclStmt(
            PtrResolvable<Type> type,
            std::string id,
            std::shared_ptr<Expression<>> value
        ) noexcept;

        void accept(Pass& visitor) override;

        [[nodiscard]] Ast getChildNodes() override;
    };
}
