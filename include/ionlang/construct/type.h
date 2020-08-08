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

        bool isPointer;

    public:
        explicit Type(std::string id, TypeKind kind = TypeKind::UserDefined, bool isPointer = false);

        void accept(Pass &visitor) override;

        TypeKind getTypeKind() const noexcept;

        bool getIsPointer() const noexcept;

        void setIsPointer(bool isPointer) noexcept;
    };
}
