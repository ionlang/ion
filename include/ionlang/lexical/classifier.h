#pragma once

#include <ionlang/misc/helpers.h>
#include <ionlang/const/token_const.h>

namespace ionlang {
    class Classifier {
    public:
        static bool isSymbol(TokenKind tokenKind);

        static bool isNumeric(TokenKind tokenKind);

        static bool isOperator(TokenKind tokenKind);

        static bool isUnsignedIntegerType(TokenKind tokenKind);

        static bool isIntegerType(TokenKind tokenKind);

        static bool isType(TokenKind tokenKind);

        static bool isKeyword(TokenKind tokenKind);

        static bool isLiteral(TokenKind tokenKind);
    };
}
