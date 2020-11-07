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

        /**
         * $
         */
        SymbolDollar,

        /**
         * #
         */
        SymbolHash,

        /**
         * (
         */
        SymbolParenthesesL,

        /**
         * )
         */
        SymbolParenthesesR,

        /**
         * [
         */
        SymbolBracketL,

        /**
         * ]
         */
        SymbolBracketR,

        /**
         * ,
         */
        SymbolComma,

        /**
         * =
         */
        SymbolEqual,

        /**
         * ;
         */
        SymbolSemiColon,

        /**
         * {
         */
        SymbolBraceL,

        /**
         * }
         */
        SymbolBraceR,

        /**
         * ->
         */
        SymbolArrow,

        /**
         * &
         */
        SymbolAmpersand,

        /**
         * @
         */
        SymbolAt,

        /**
         * ...
         */
        SymbolEllipsis,

        /**
         * ::
         */
        SymbolScope,

        KeywordFunction,

        KeywordExtern,

        KeywordIf,

        KeywordElse,

        KeywordModule,

        KeywordGlobal,

        KeywordReturn,

        KeywordUnsafe,

        KeywordStruct,

        KeywordLet,

        KeywordImport,

        KeywordIntrinsic,

        KeywordConstructor,

        KeywordDestructor,

        KeywordOperator,

        TypeVoid,

        TypeBool,

        TypeInt8,

        TypeInt16,

        TypeInt32,

        TypeInt64,

        TypeUnsignedInt8,

        TypeUnsignedInt16,

        TypeUnsignedInt32,

        TypeUnsignedInt64,

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

    std::ostream &operator<<(std::ostream& stream, const TokenKind& tokenKind);
}
