#include <ionlang/lexical/token.h>
#include "pch.h"

using namespace ionlang;

TEST(TokenTest, CorrectProperties) {
    Token token = Token(TokenKind::Identifier, std::string("hello_world"), 123);

    EXPECT_EQ(token.value, "hello_world");
    EXPECT_EQ(token.kind, TokenKind::Identifier);
    EXPECT_EQ(token.startPosition, 123);

    // End position must be 123 + len("hello_world") (11) = 134.
    EXPECT_EQ(token.getEndPosition(), 134);
}

TEST(TokenTest, EqualityAndDifference) {
    // Create test tokens.
    Token token1 = Token(TokenKind::SymbolAmpersand, "&", 5);
    Token token2 = Token(TokenKind::SymbolAmpersand, "&", 5);
    Token token3 = Token(TokenKind::SymbolAmpersand, "~", 5);

    // Expect equal.
    EXPECT_EQ(token1, token2);
    EXPECT_EQ(token2, token1);

    // Expect not to equal.
    EXPECT_NE(token1, token3);
    EXPECT_NE(token2, token3);
}
