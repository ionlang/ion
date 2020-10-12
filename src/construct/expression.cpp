#include <ionlang/passes/pass.h>

namespace ionlang {
    Expression::Expression(ExpressionKind kind, PtrResolvable<Type> type) :
        Construct(ConstructKind::Expression),
        expressionKind(kind),
        type(std::move(type)) {
        //
    }
}
