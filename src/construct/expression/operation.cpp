#include <ionlang/passes/pass.h>

namespace ionlang {
    std::shared_ptr<OperationExpr> OperationExpr::make(
        const PtrResolvable<Type>& type,
        IntrinsicOperatorKind operation,
        const std::shared_ptr<Expression<>>& leftSideValue,
        ionshared::OptPtr<Expression<>> rightSideValue
    ) noexcept {
        std::shared_ptr<OperationExpr> result = std::make_shared<OperationExpr>(
            type,
            operation,
            leftSideValue,
            rightSideValue
        );

        // TODO: What about type?

        leftSideValue->setParent(result);

        if (ionshared::util::hasValue(rightSideValue)) {
            rightSideValue->get()->setParent(result);
        }

        return result;
    }

    OperationExpr::OperationExpr(
        PtrResolvable<Type> type,
        IntrinsicOperatorKind operation,
        std::shared_ptr<Expression<>> leftSideValue,
        ionshared::OptPtr<Expression<>> rightSideValue
    ) noexcept :
        Expression<>(ExpressionKind::Operation, std::move(type)),
        operation(operation),
        leftSideValue(std::move(leftSideValue)),
        rightSideValue(std::move(rightSideValue)) {
        //
    }

    void OperationExpr::accept(Pass& visitor) {
        visitor.visitOperationExpr(this->dynamicCast<OperationExpr>());
    }

    Ast OperationExpr::getChildNodes() {
        // TODO: What about type?

        Ast children{this->leftSideValue};

        if (ionshared::util::hasValue(this->rightSideValue)) {
            children.push_back(*this->rightSideValue);
        }

        return children;
    }

    bool OperationExpr::isBinary() const noexcept {
        return ionshared::util::hasValue(this->rightSideValue);
    }
}
