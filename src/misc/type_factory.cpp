#include <ionlang/misc/type_factory.h>

namespace ionlang::type_factory {
    ionshared::Ptr<IntegerType> typeInteger(IntegerKind integerKind, bool isSigned) {
        return std::make_shared<IntegerType>(integerKind, isSigned);
    }

    ionshared::Ptr<BooleanType> typeBoolean() {
        return std::make_shared<BooleanType>();
    }

    ionshared::Ptr<IntegerType> typeChar() {
        return type_factory::typeInteger(IntegerKind::Int8, false);
    }

    ionshared::Ptr<Type> typeString() {
        // TODO: Awaiting array types support.
        return type_factory::typeChar();
    }

    ionshared::Ptr<VoidType> typeVoid() {
        return std::make_shared<VoidType>();
    }
}
