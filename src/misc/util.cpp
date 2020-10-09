#include <ionlang/misc/util.h>
#include <ionlang/const/const_name.h>
#include <ionlang/const/grammar.h>
#include <ionlang/construct/function.h>
#include <ionlang/construct/extern.h>
#include <ionlang/construct/struct.h>

namespace ionlang::util {
    std::string resolveIntegerKindName(IntegerKind kind) {
        switch (kind) {
            case IntegerKind ::Int8: {
                return const_name::typeInt8;
            }

            case IntegerKind::Int16: {
                return const_name::typeInt16;
            }

            case IntegerKind::Int32: {
                return const_name::typeInt32;
            }

            case IntegerKind::Int64: {
                return const_name::typeInt64;
            }

            default: {
                throw std::runtime_error("Unknown integer kind");
            }
        }
    }

    TypeKind resolveTypeKind(const std::string& id) {
        // TODO: CRITICAL: Add support new/missing types.

        if (id == const_name::typeInt8) {
            return TypeKind::Integer;
        }
        else if (id == const_name::typeInt16) {
            return TypeKind::Integer;
        }
        else if (id == const_name::typeInt32) {
            return TypeKind::Integer;
        }
        else if (id == const_name::typeInt64) {
            return TypeKind::Integer;
        }
        else if (id == const_name::typeVoid) {
            return TypeKind::Void;
        }
        else if (id == const_name::typeString) {
            return TypeKind::String;
        }

        return TypeKind::UserDefined;
    }

    std::optional<IntegerKind> calculateIntegerKindFromBitLength(uint32_t bitLength) noexcept {
        if (bitLength <= 8) {
            return IntegerKind::Int8;
        }
        else if (bitLength <= 16) {
            return IntegerKind::Int16;
        }
        else if (bitLength <= 32) {
            return IntegerKind::Int32;
        }
        else if (bitLength <= 64) {
            return IntegerKind::Int64;
        }
        else if (bitLength <= 128) {
            return IntegerKind::Int128;
        }

        return std::nullopt;
    }

    std::optional<std::string> findConstructId(const ionshared::Ptr<Construct>& construct) {
        ConstructKind constructKind = construct->constructKind;

        /**
         * If the construct derives from ionshared::Named, simply cast it
         * and get it's id. Certain constructs do derive from it.
         */
        if (constructKind == ConstructKind::Prototype || constructKind == ConstructKind::Global) {
            return construct->dynamicCast<ionshared::Named>()->name;
        }

        // Otherwise, handle other specific cases.
        switch (constructKind) {
            case ConstructKind::Function: {
                return construct->dynamicCast<Function>()->prototype->name;
            }

            case ConstructKind::Extern: {
                return construct->dynamicCast<Extern>()->prototype->name;
            }

            case ConstructKind::Struct: {
                return construct->dynamicCast<Struct>()->name;
            }

            case ConstructKind::Statement: {
                return util::findStatementId(construct->dynamicCast<Statement>());
            }

            // TODO: Make sure there aren't any more that should be here.

            default: {
                return std::nullopt;
            }
        }
    }

    std::optional<std::string> findStatementId(const ionshared::Ptr<Statement>& statement) noexcept {
        // TODO: Implement. Check for derivations from ionshared::Named first, then specific cases (similar to util::findConstructId()).
        // TODO: VariableDecl can easily be implemented as derived.
        switch (statement->statementKind) {
            default: {
                return std::nullopt;
            }
        }
    }

    std::optional<IntrinsicOperatorKind> findIntrinsicOperatorKind(TokenKind tokenKind) {
        switch (tokenKind) {
            case TokenKind::OperatorAddition: {
                return IntrinsicOperatorKind::Addition;
            }

            case TokenKind::OperatorSubtraction: {
                return IntrinsicOperatorKind::Subtraction;
            }

            case TokenKind::OperatorMultiplication: {
                return IntrinsicOperatorKind::Multiplication;
            }

            case TokenKind::OperatorDivision: {
                return IntrinsicOperatorKind::Division;
            }

            case TokenKind::OperatorExponent: {
                return IntrinsicOperatorKind::Exponent;
            }

            case TokenKind::OperatorModulo: {
                return IntrinsicOperatorKind::Modulo;
            }

            case TokenKind::OperatorLessThan: {
                return IntrinsicOperatorKind::LessThan;
            }

            case TokenKind::OperatorGreaterThan: {
                return IntrinsicOperatorKind::GreaterThan;
            }

            default: {
                return std::nullopt;
            }
        }
    }

    std::optional<uint32_t> findIntrinsicOperatorKindPrecedence(TokenKind tokenKind) {
        // TODO

        return std::nullopt;
    }
}
