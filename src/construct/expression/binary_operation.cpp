#include <ionlang/passes/pass.h>

namespace ionlang {
    BinaryOperation::BinaryOperation(const BinaryOperationOpts &opts) :
        Expression(ExpressionKind::BinaryOperation, opts.type),
        operation(opts.operation),
        leftSide(opts.leftSide),
        rightSide(opts.rightSide) {
        //
    }

    Operator BinaryOperation::getOperator() const noexcept {
        return this->operation;
    }

    void BinaryOperation::setOperator(Operator operation) noexcept {
        this->operation = operation;
    }

    ionshared::Ptr<Construct> BinaryOperation::getLeftSide() const noexcept {
        return this->leftSide;
    }

    void BinaryOperation::setLeftSide(ionshared::Ptr<Construct> leftSide) noexcept {
        this->leftSide = std::move(leftSide);
    }

    ionshared::OptPtr<Construct> BinaryOperation::getRightSide() const noexcept {
        return this->rightSide;
    }

    void BinaryOperation::setRightSide(ionshared::OptPtr<Construct> rightSide) noexcept {
        this->rightSide = std::move(rightSide);
    }

    bool BinaryOperation::hasRightSide() const noexcept {
        return ionshared::util::hasValue(this->rightSide);
    }
}
