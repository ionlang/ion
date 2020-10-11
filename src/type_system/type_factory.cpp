#include <ionlang/type_system/type_factory.h>

namespace ionlang::type_factory {
    ionshared::Ptr<IntegerType> typeInteger(IntegerKind integerKind, bool isSigned) {
        return std::make_shared<IntegerType>(integerKind, isSigned);
    }

    ionshared::Ptr<IntegerType> typeInteger8(bool isSigned) {
        return type_factory::typeInteger(IntegerKind::Int8, isSigned);
    }

    ionshared::Ptr<IntegerType> typeInteger16(bool isSigned) {
        return type_factory::typeInteger(IntegerKind::Int16, isSigned);
    }

    ionshared::Ptr<IntegerType> typeInteger32(bool isSigned) {
        return type_factory::typeInteger(IntegerKind::Int32, isSigned);
    }

    ionshared::Ptr<IntegerType> typeInteger64(bool isSigned) {
        return type_factory::typeInteger(IntegerKind::Int64, isSigned);
    }

    ionshared::Ptr<IntegerType> typeInteger128(bool isSigned) {
        return type_factory::typeInteger(IntegerKind::Int128, isSigned);
    }

    ionshared::Ptr<BooleanType> typeBoolean() {
        return std::make_shared<BooleanType>();
    }

    ionshared::Ptr<IntegerType> typeChar() {
        return type_factory::typeInteger8(false);
    }

    ionshared::Ptr<IntegerType> typeString() {
        // TODO: Awaiting array types support.
        return type_factory::typeChar();
    }

    ionshared::Ptr<VoidType> typeVoid() {
        return std::make_shared<VoidType>();
    }
}
