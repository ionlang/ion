#pragma once

#include <iostream>

namespace ionlang {
    enum class TokenKind {
        Unknown,

        Identifier,

        Whitespace,

        LiteralString,

        LiteralDecimal,

        LiteralInteger,

        LiteralCharacter,

        LiteralBoolean,

        SymbolDollar,

        SymbolHash,

        SymbolParenthesesL,

        SymbolParenthesesR,

        SymbolBracketL,

        SymbolBracketR,

        SymbolComma,

        SymbolEqual,

        SymbolSemiColon,

        SymbolStar,

        SymbolBraceL,

        SymbolBraceR,

        SymbolArrow,

        SymbolAmpersand,

        KeywordFunction,

        KeywordExtern,

        KeywordIf,

        KeywordElse,

        KeywordModule,

        KeywordGlobal,

        KeywordReturn,

        KeywordUnsafe,

        TypeVoid,

        TypeBool,

        TypeInt8,

        TypeInt16,

        TypeInt32,

        TypeInt64,

        TypeFloat16,

        TypeFloat32,

        TypeFloat64,

        TypeChar,

        TypeString,

        QualifierConst,

        QualifierMutable,

        OperatorAddition,

        OperatorSubtraction,

        OperatorMultiplication,

        OperatorDivision,

        OperatorModulo,

        OperatorExponent,

        OperatorGreaterThan,

        OperatorLessThan
    };

    std::ostream &operator<<(std::ostream &stream, const TokenKind &tokenKind);
}
