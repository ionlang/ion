#include <utility>
#include <ionlang/passes/pass.h>

namespace ionlang {
    Type::Type(
        std::string name,
        TypeKind kind,
        ionshared::Ptr<TypeQualifiers> qualifiers
    ) noexcept :
        Construct(ConstructKind::Type),
        ionshared::Named{std::move(name)},
        typeKind(kind),
        qualifiers(std::move(qualifiers)) {
        //
    }
}
