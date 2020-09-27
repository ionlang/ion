#pragma once

#include <ionshared/misc/helpers.h>
#include <ionlang/construct/type/integer_type.h>
#include <ionlang/construct/value.h>

namespace ionlang {
    class Pass;

    struct IntegerLiteral : Value<IntegerType> {
        int64_t value;

        IntegerLiteral(ionshared::Ptr<IntegerType> type, int64_t value);

        void accept(Pass &visitor) override;
    };
}
