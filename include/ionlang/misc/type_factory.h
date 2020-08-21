#pragma once

#include <ionlang/construct/type/integer_type.h>
#include <ionlang/construct/type/boolean_type.h>
#include <ionlang/construct/type/void_type.h>
#include <ionlang/construct/type/integer_type.h>

namespace ionlang {
    class TypeFactory {
    public:
        static ionshared::Ptr<IntegerType> typeInteger(IntegerKind integerKind, bool isSigned = true);

        static ionshared::Ptr<BooleanType> typeBoolean();

        static ionshared::Ptr<IntegerType> typeChar();

        static ionshared::Ptr<Type> typeString();

        static ionshared::Ptr<VoidType> typeVoid();
    };
}
