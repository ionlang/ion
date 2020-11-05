#include <ionlang/passes/pass.h>

namespace ionlang {
    Attribute::Attribute(std::string id) :
        ConstructWithParent<Module, Construct, ConstructKind>(ConstructKind::Attribute),
        ionshared::Named{std::move(id)} {
        //
    }

    void Attribute::accept(Pass& visitor) {
        visitor.visitAttribute(this->dynamicCast<Attribute>());
    }
}
