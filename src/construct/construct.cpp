#include <ionlang/passes/pass.h>

namespace ionlang {
    Construct::Construct(ConstructKind kind) : ionshared::BaseConstruct<Construct, ConstructKind>(kind) {
        //
    }

    void Construct::accept(ionshared::BasePass<Construct> visitor) {
        // TODO
    }

    Ast Construct::getChildNodes() {
        // By default, construct contains no children.
        return {};
    }
}
