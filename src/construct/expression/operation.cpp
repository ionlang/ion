#include <ionlang/passes/pass.h>

namespace ionlang {
    OperationExpr::OperationExpr(const OperationExprOpts& opts) noexcept :
        Expression<>(ExpressionKind::Operation, opts.type),
        operation(opts.operation),
        leftSideValue(opts.leftSideValue),
        rightSideValue(opts.rightSideValue) {
        //
    }

    void OperationExpr::accept(Pass& visitor) {
        visitor.visitOperationExpr(this->dynamicCast<OperationExpr>());
    }

    Ast OperationExpr::getChildNodes() {
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
