#include <ionlang/passes/pass.h>

namespace ionlang {
    Attribute::Attribute(ionshared::Ptr<Construct> parent, std::string id) :
        ChildConstruct<>(std::move(parent), ConstructKind::Attribute),
        ionshared::Named(std::move(id)) {
        //
    }

    void Attribute::accept(Pass &visitor) {
        visitor.visitAttribute(this->dynamicCast<Attribute>());
    }
}
