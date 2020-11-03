#pragma once

#include <ionshared/misc/helpers.h>
#include <ionlang/construct/type/integer_type.h>
#include <ionlang/construct/expression.h>

namespace ionlang {
    struct Pass;

    struct IntegerLiteral : Expression<IntegerType> {
        static std::shared_ptr<IntegerLiteral> make(
            const std::shared_ptr<IntegerType>& type,
            int64_t value
        ) noexcept;

        int64_t value;

        IntegerLiteral(std::shared_ptr<IntegerType> type, int64_t value) noexcept;

        void accept(Pass& visitor) override;
    };
}
