#pragma once

#include <ionlang/construct/type.h>

namespace ionlang {
    class Pass;

    enum class ValueKind {
        Integer,

        Character,

        String,

        Boolean,

        Expression
    };

    template<typename T = Type>
    struct Value : public Construct {
        const ValueKind kind;

        const ionshared::Ptr<T> type;

        Value() = delete;

        Value(ValueKind kind, ionshared::Ptr<T> type) :
            Construct(ConstructKind::Value),
            kind(kind),
            type(type) {
            //
        }

        void accept(Pass &visitor) {
            // TODO: CRITICAL: Fix 'incomplete type' problem.
            //visitor.visitValue(this->dynamicCast<Value<>>());
        }

        ValueKind getValueKind() const noexcept {
            return this->kind;
        }

        ionshared::Ptr<T> getType() const noexcept {
            return this->type;
        }
    };
}
