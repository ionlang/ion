#pragma once

#include <iostream>

namespace ionlang {
    enum struct TokenKind {
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

        SymbolBraceL,

        SymbolBraceR,

        SymbolArrow,

        SymbolAmpersand,

        SymbolAt,

        SymbolEllipsis,

        KeywordFunction,

        KeywordExtern,

        KeywordIf,

        KeywordElse,

        KeywordModule,

        KeywordGlobal,

        KeywordReturn,

        KeywordUnsafe,

        KeywordStruct,

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

        OperatorGreaterThan,

        OperatorLessThan,

        Comment
    };

    std::ostream &operator<<(std::ostream &stream, const TokenKind &tokenKind);
}
