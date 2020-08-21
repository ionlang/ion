#include <ionlang/misc/type_factory.h>

namespace ionlang {
    ionshared::Ptr<IntegerType> TypeFactory::typeInteger(IntegerKind integerKind, bool isSigned) {
        return std::make_shared<IntegerType>(integerKind, isSigned);
    }

    ionshared::Ptr<BooleanType> TypeFactory::typeBoolean() {
        return std::make_shared<BooleanType>();
    }

    ionshared::Ptr<IntegerType> TypeFactory::typeChar() {
        return TypeFactory::typeInteger(IntegerKind::Int8, false);
    }

    ionshared::Ptr<Type> TypeFactory::typeString() {
        // TODO: Awaiting array types support.
        return TypeFactory::typeChar();
    }

    ionshared::Ptr<VoidType> TypeFactory::typeVoid() {
        return std::make_shared<VoidType>();
    }
}
