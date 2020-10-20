#include <ionlang/type_system/type_factory.h>

namespace ionlang::type_factory {
    std::shared_ptr<IntegerType> typeInteger(IntegerKind integerKind, bool isSigned) {
        return std::make_shared<IntegerType>(integerKind, isSigned);
    }

    std::shared_ptr<IntegerType> typeInteger8(bool isSigned) {
        return type_factory::typeInteger(IntegerKind::Int8, isSigned);
    }

    std::shared_ptr<IntegerType> typeInteger16(bool isSigned) {
        return type_factory::typeInteger(IntegerKind::Int16, isSigned);
    }

    std::shared_ptr<IntegerType> typeInteger32(bool isSigned) {
        return type_factory::typeInteger(IntegerKind::Int32, isSigned);
    }

    std::shared_ptr<IntegerType> typeInteger64(bool isSigned) {
        return type_factory::typeInteger(IntegerKind::Int64, isSigned);
    }

    std::shared_ptr<IntegerType> typeInteger128(bool isSigned) {
        return type_factory::typeInteger(IntegerKind::Int128, isSigned);
    }

    std::shared_ptr<BooleanType> typeBoolean() {
        return std::make_shared<BooleanType>();
    }

    std::shared_ptr<IntegerType> typeChar() {
        return type_factory::typeInteger8(false);
    }

    std::shared_ptr<IntegerType> typeString() {
        // TODO: Awaiting array types support.
        return type_factory::typeChar();
    }

    std::shared_ptr<VoidType> typeVoid() {
        return std::make_shared<VoidType>();
    }
}
