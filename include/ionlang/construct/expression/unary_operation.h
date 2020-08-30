#pragma once

#include <ionlang/construct/expression.h>

namespace ionlang {
    class Pass;

    enum class Operator {
        Addition,

        Subtraction,

        Multiplication,

        Division,

        Exponent,

        Modulo,

        LessThan,

        GreaterThan
    };

    class UnaryOperation : public Expression {
    private:
        Operator operation;

        ionshared::Ptr<Construct> value;

    public:
        UnaryOperation(
            ionshared::Ptr<Type> type,
            Operator operation,
            ionshared::Ptr<Construct> value
        );

        [[nodiscard]] Operator getOperator() const noexcept;

        void setOperator(Operator operation) noexcept;

        [[nodiscard]] ionshared::Ptr<Construct> getValue() const noexcept;

        void setValue(ionshared::Ptr<Construct> value) noexcept;
    };
}
