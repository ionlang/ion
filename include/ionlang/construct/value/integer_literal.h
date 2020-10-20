#pragma once

#include <ionshared/misc/helpers.h>
#include <ionlang/construct/type/integer_type.h>
#include <ionlang/construct/expression.h>

namespace ionlang {
    struct Pass;

    struct IntegerLiteral : Expression<IntegerType> {
        int64_t value;

        IntegerLiteral(std::shared_ptr<IntegerType> type, int64_t value);

        void accept(Pass &visitor) override;
    };
}
