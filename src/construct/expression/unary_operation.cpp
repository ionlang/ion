#include <ionlang/passes/pass.h>

namespace ionlang {
    UnaryOperation::UnaryOperation(
        PtrResolvable<Type> type,
        IntrinsicOperatorKind operation,
        ionshared::Ptr<Construct> value
    ) :
        Expression(ExpressionKind::UnaryOperation, std::move(type)),
        operation(operation),
        value(std::move(value)) {
    }
}
