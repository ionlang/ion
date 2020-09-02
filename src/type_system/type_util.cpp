#include <ionlang/type_system/type_util.h>
#include <ionlang/construct/type/integer_type.h>

namespace ionlang::type_util {
    bool isAtomicTypesCompatible(TypeKind typeKindA, TypeKind typeKindB) {
        if (typeKindA == TypeKind::UserDefined || typeKindB == TypeKind::UserDefined) {
            throw std::invalid_argument("Neither argument may be user-defined type kind");
        }
        // Void is incompatible with everything.
        else if (typeKindA == TypeKind::Void || typeKindB == TypeKind::Void) {
            return false;
        }
        // Same, non-void atomic types. They are compatible.
        else if (typeKindA == typeKindB) {
            return true;
        }

        // TODO: Missing additional logic. Refer to (https://github.com/DoctorWkt/acwj/tree/master/12_Types_pt1#building-binary-expressions-comparing-types).

        // Anything remaining is compatible.
        return true;
    }

    bool isSameType(const ionshared::Ptr<Type> &typeA, const ionshared::Ptr<Type> &typeB) {
        TypeKind typeAKind = typeA->getTypeKind();
        TypeKind typeBKind = typeB->getTypeKind();

        if (typeAKind != typeBKind) {
            return false;
        }

        switch (typeAKind) {
            case TypeKind::Integer: {
                ionshared::Ptr<IntegerType> integerTypeA = typeA->dynamicCast<IntegerType>();
                ionshared::Ptr<IntegerType> integerTypeB = typeB->dynamicCast<IntegerType>();

                return integerTypeA->getIntegerKind() == integerTypeB->getIntegerKind();
            }

            // TODO: Decimal types as well (copy integer code basically).

            case TypeKind::UserDefined: {
                // TODO

                throw std::runtime_error("Not implemented");

                break;
            }

            default: {
                return true;
            }
        }
    }
}
