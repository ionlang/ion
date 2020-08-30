#include <ionlang/passes/pass.h>

namespace ionlang {
    UnaryOperation::UnaryOperation(
        ionshared::Ptr<Type> type,
        Operator operation,
        ionshared::Ptr<Construct> value
    ) :
        Expression(ExpressionKind::UnaryOperation, std::move(type)),
        operation(operation),
        value(std::move(value)) {
    }

    Operator UnaryOperation::getOperator() const noexcept {
        return this->operation;
    }

    void UnaryOperation::setOperator(Operator operation) noexcept {
        this->operation = operation;
    }

    ionshared::Ptr<Construct> UnaryOperation::getValue() const noexcept {
        return this->value;
    }

    void UnaryOperation::setValue(ionshared::Ptr<Construct> value) noexcept {
        this->value = std::move(value);
    }
}
