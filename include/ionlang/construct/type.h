#pragma once

#include <ionshared/container/set.h>
#include <ionshared/misc/named.h>
#include "construct.h"

namespace ionlang {
    enum class TypeQualifier {
        Constant,

        Mutable,

        Reference,

        Pointer
    };

    enum class TypeKind {
        UserDefined,

        Void,

        Integer,

        String,

        Boolean
    };

    typedef ionshared::Set<TypeQualifier> TypeQualifiers;

    class Type : public Construct, public ionshared::Named {
    private:
        TypeKind kind;

        ionshared::Ptr<TypeQualifiers> qualifiers;

    public:
        explicit Type(
            std::string id,
            TypeKind kind = TypeKind::UserDefined,
            ionshared::Ptr<TypeQualifiers> qualifiers = std::make_shared<TypeQualifiers>()
        );

        void accept(Pass &visitor) override;

        TypeKind getTypeKind() const noexcept;

        [[nodiscard]] ionshared::Ptr<TypeQualifiers> getQualifiers() const noexcept;

        void setQualifiers(ionshared::Ptr<TypeQualifiers> qualifiers) noexcept;

        [[nodiscard]] bool addQualifier(TypeQualifier qualifier) noexcept;

        [[nodiscard]] bool removeQualifier(TypeQualifier qualifier) noexcept;

        [[nodiscard]] bool hasQualifier(TypeQualifier qualifier) const;
    };
}
