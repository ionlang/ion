#pragma once

#include <ionlang/misc/helpers.h>
#include <ionlang/const/grammar.h>

namespace ionlang {
    struct Classifier {
        [[nodiscard]] static bool isSymbol(TokenKind tokenKind);

        [[nodiscard]] static bool isNumeric(TokenKind tokenKind);

        [[nodiscard]] static bool isIntrinsicOperator(TokenKind tokenKind);

        [[nodiscard]] static bool isUnsignedIntegerType(TokenKind tokenKind);

        [[nodiscard]] static bool isIntegerType(TokenKind tokenKind);

        [[nodiscard]] static bool isBuiltInType(TokenKind tokenKind);

        [[nodiscard]] static bool isKeyword(TokenKind tokenKind);

        [[nodiscard]] static bool isLiteral(TokenKind tokenKind);

        [[nodiscard]] static bool isStatement(
            TokenKind tokenKind,
            std::optional<TokenKind> nextTokenKind
        );

        [[nodiscard]] static bool isMethodOrFunction(
            TokenKind tokenKind
        );
    };
}
