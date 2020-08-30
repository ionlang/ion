#pragma once

#include <ionlang/construct/type/integer_type.h>
#include <ionlang/construct/type/boolean_type.h>
#include <ionlang/construct/type/void_type.h>
#include <ionlang/construct/type/integer_type.h>

namespace ionlang::type_factory {
    ionshared::Ptr<IntegerType> typeInteger(IntegerKind integerKind, bool isSigned = true);

    ionshared::Ptr<BooleanType> typeBoolean();

    ionshared::Ptr<IntegerType> typeChar();

    ionshared::Ptr<Type> typeString();

    ionshared::Ptr<VoidType> typeVoid();
}
