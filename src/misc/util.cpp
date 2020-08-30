#include <ionlang/misc/util.h>
#include <ionlang/const/const_name.h>
#include <ionlang/construct/function.h>
#include <ionlang/construct/extern.h>

namespace ionlang::util {
    std::string resolveIntegerKindName(IntegerKind kind) {
        switch (kind) {
            case IntegerKind ::Int8: {
                return ConstName::typeInt8;
            }

            case IntegerKind::Int16: {
                return ConstName::typeInt16;
            }

            case IntegerKind::Int32: {
                return ConstName::typeInt32;
            }

            case IntegerKind::Int64: {
                return ConstName::typeInt64;
            }

            default: {
                throw std::runtime_error("Unknown integer kind");
            }
        }
    }

    TypeKind resolveTypeKind(const std::string &id) {
        // TODO: CRITICAL: Add support new/missing types.

        if (id == ConstName::typeInt8) {
            return TypeKind::Integer;
        }
        else if (id == ConstName::typeInt16) {
            return TypeKind::Integer;
        }
        else if (id == ConstName::typeInt32) {
            return TypeKind::Integer;
        }
        else if (id == ConstName::typeInt64) {
            return TypeKind::Integer;
        }
        else if (id == ConstName::typeVoid) {
            return TypeKind::Void;
        }
        else if (id == ConstName::typeString) {
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

    std::optional<std::string> findConstructId(const ionshared::Ptr<Construct> &construct) {
        ConstructKind constructKind = construct->getConstructKind();

        /**
         * If the construct derives from ionshared::Named, simply cast it
         * and get it's id. Certain constructs do derive from it.
         */
        if (constructKind == ConstructKind::Prototype || constructKind == ConstructKind::Global) {
            return construct->dynamicCast<ionshared::Named>()->getId();
        }

        // Otherwise, handle other specific cases.
        switch (constructKind) {
            case ConstructKind::Function: {
                return construct->dynamicCast<Function>()->getPrototype()->getId();
            }

            case ConstructKind::Extern: {
                return construct->dynamicCast<Extern>()->getPrototype()->getId();
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

    std::optional<std::string> findStatementId(const ionshared::Ptr<Statement> &statement) noexcept {
        // TODO: Implement. Check for derivations from ionshared::Named first, then specific cases (similar to util::findConstructId()).
        // TODO: VariableDecl can easily be implemented as derived.
        switch (statement->getStatementKind()) {
            default: {
                return std::nullopt;
            }
        }
    }

    std::optional<Operator> findOperator(TokenKind tokenKind) {
        switch (tokenKind) {
            case TokenKind::OperatorAddition: {
                return Operator::Addition;
            }

            case TokenKind::OperatorSubtraction: {
                return Operator::Subtraction;
            }

            case TokenKind::OperatorMultiplication: {
                return Operator::Multiplication;
            }

            case TokenKind::OperatorDivision: {
                return Operator::Division;
            }

            case TokenKind::OperatorExponent: {
                return Operator::Exponent;
            }

            case TokenKind::OperatorModulo: {
                return Operator::Modulo;
            }

            case TokenKind::OperatorLessThan: {
                return Operator::LessThan;
            }

            case TokenKind::OperatorGreaterThan: {
                return Operator::GreaterThan;
            }

            default: {
                return std::nullopt;
            }
        }
    }
}
