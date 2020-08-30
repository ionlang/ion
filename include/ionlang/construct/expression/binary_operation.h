#pragma once

#include <ionlang/construct/expression.h>
#include "unary_operation.h"

namespace ionlang {
    class Pass;

    struct BinaryOperationOpts {
        ionshared::Ptr<Type> type;

        Operator operation;

        ionshared::Ptr<Construct> leftSide;

        ionshared::OptPtr<Construct> rightSide;
    };

    class BinaryOperation : public Expression {
    private:
        Operator operation;

        ionshared::Ptr<Construct> leftSide;

        ionshared::OptPtr<Construct> rightSide;

    public:
        explicit BinaryOperation(const BinaryOperationOpts &opts);

        [[nodiscard]] Operator getOperator() const noexcept;

        void setOperator(Operator operation) noexcept;

        [[nodiscard]] ionshared::Ptr<Construct> getLeftSide() const noexcept;

        void setLeftSide(ionshared::Ptr<Construct> leftSide) noexcept;

        [[nodiscard]] ionshared::OptPtr<Construct> getRightSide() const noexcept;

        void setRightSide(ionshared::OptPtr<Construct> rightSide) noexcept;

        [[nodiscard]] bool hasRightSide() const noexcept;
    };
}
