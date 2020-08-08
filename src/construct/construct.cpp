#include <ionlang/construct/construct.h>

namespace ionlang {
    Construct::Construct(ConstructKind kind) : ionshared::BareConstruct<Construct, ConstructKind>(kind) {
        //
    }

    Ast Construct::getChildNodes() {
        // By default, construct contains no children.
        return {};
    }
}
