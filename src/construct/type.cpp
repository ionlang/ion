#include <utility>
#include <ionlang/passes/pass.h>

namespace ionlang {
    Type::Type(
        std::string name,
        TypeKind kind,
        std::shared_ptr<TypeQualifierSet> qualifiers
    ) noexcept :
        Construct(ConstructKind::Type),
        typeName(name),
        typeKind(kind),
        qualifiers(std::move(qualifiers)) {
        //
    }
}
