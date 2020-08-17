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

        KeywordFunction,

        KeywordExtern,

        KeywordIf,

        KeywordElse,

        KeywordMutable,

        KeywordModule,

        KeywordGlobal,

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

        OperatorAdd,

        OperatorSubtract,

        OperatorMultiply,

        OperatorDivide,

        OperatorModulo,

        OperatorExponent,

        OperatorGreaterThan,

        OperatorLessThan
    };

    std::ostream &operator<<(std::ostream &stream, const TokenKind &tokenKind);
}
