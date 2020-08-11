#pragma once

#include <ionshared/misc/helpers.h>
#include <ionlang/construct/type/integer_type.h>
#include <ionlang/construct/value.h>

namespace ionlang {
    class Pass;

    class IntegerValue : public Value<IntegerType> {
    private:
        int64_t value;

    public:
        IntegerValue(ionshared::Ptr<IntegerType> type, int64_t value);

        void accept(Pass &visitor) override;

        int64_t getValue() const;

        void setValue(int64_t value);
    };
}
