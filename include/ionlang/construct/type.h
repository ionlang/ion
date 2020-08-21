#pragma once

#include <ionshared/misc/named.h>
#include "construct.h"

namespace ionlang {
    enum class TypeKind {
        UserDefined,

        Void,

        Integer,

        String,

        Boolean
    };

    class Type : public Construct, public ionshared::Named {
    private:
        TypeKind kind;

    public:
        explicit Type(std::string id, TypeKind kind = TypeKind::UserDefined);

        void accept(Pass &visitor) override;

        TypeKind getTypeKind() const noexcept;
    };
}
