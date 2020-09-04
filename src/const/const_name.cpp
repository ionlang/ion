#include <ionlang/const/const_name.h>

namespace ionlang {
    const std::string ConstName::anonymous = "anonymous";

    const std::string ConstName::main = "main";

    // TODO: Bools are also hard-coded on it's corresponding regex.
    const std::string ConstName::booleanTrue = "true";

    const std::string ConstName::booleanFalse = "false";

    const std::string ConstName::typeVoid = "void";

    const std::string ConstName::typeBool = "bool";

    const std::string ConstName::typeInt8 = "i8";

    const std::string ConstName::typeInt16 = "i16";

    const std::string ConstName::typeInt32 = "i32";

    const std::string ConstName::typeInt64 = "i64";

    const std::string ConstName::typeUnsignedInt16 = "ui16";

    const std::string ConstName::typeUnsignedInt32 = "ui32";

    const std::string ConstName::typeUnsignedInt64 = "ui64";

    const std::string ConstName::typeFloat16 = "f16";

    const std::string ConstName::typeFloat32 = "f32";

    const std::string ConstName::typeFloat64 = "f64";

    const std::string ConstName::typeChar = "char";

    const std::string ConstName::typeString = "str";

    const std::string ConstName::statementReturn = "return";

    const std::string ConstName::statementIf = "if";
}
