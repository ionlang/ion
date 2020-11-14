#pragma once

#include <ionlang/construct/type.h>

namespace ionlang {
    struct Pass;

    enum struct IntegerKind {
        Int8 = 8,

        Int16 = 16,

        Int32 = 32,

        Int64 = 64,

        Int128 = 128
    };

    struct IntegerType : Type {
        const IntegerKind integerKind;

        bool isSigned;

        explicit IntegerType(
            IntegerKind kind,
            bool isSigned = true,

            std::shared_ptr<TypeQualifierSet> qualifiers =
                std::make_shared<TypeQualifierSet>()
        );

        void accept(Pass& pass) override;
    };
}
