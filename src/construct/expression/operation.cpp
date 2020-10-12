#include <ionlang/passes/pass.h>

namespace ionlang {
    OperationExpr::OperationExpr(const OperationExprOpts& opts) :
        Expression(ExpressionKind::Operation, opts.type),
        operation(opts.operation),
        leftSide(opts.leftSide),
        rightSide(opts.rightSide) {
        //
    }
}
