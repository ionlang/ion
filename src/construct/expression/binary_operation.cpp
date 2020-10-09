#include <ionlang/passes/pass.h>

namespace ionlang {
    BinaryOperation::BinaryOperation(const BinaryOperationOpts &opts) :
        Expression(ExpressionKind::BinaryOperation, opts.type),
        operation(opts.operation),
        leftSide(opts.leftSide),
        rightSide(opts.rightSide) {
        //
    }
}
