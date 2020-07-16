#include <ionlang/lexical/token.h>

namespace ionlang {
    Token::Token(TokenKind kind, std::string value, uint32_t startPosition, uint32_t lineNumber)
        : BareToken(kind, value, startPosition, lineNumber) {
        //
    }

    std::ostream &operator<<(std::ostream &stream, Token &token) {
        // TODO: Include line number as well.
        return stream << "Token(" << token.getValue() << ", " << token.getKind() << ", " << token.getStartPosition()
            << "-" << token.getEndPosition() << ")";
    }
}
