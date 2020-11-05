#include <utility>
#include <ionlang/passes/pass.h>

namespace ionlang {
    Type::Type(
        std::string name,
        TypeKind kind,
        std::shared_ptr<TypeQualifiers> qualifiers
    ) noexcept :
        Construct(ConstructKind::Type),
        typeName(name),
        typeKind(kind),
        qualifiers(std::move(qualifiers)) {
        //
    }
}
