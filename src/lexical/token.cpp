#include <ionlang/lexical/token.h>

namespace ionlang {
    std::ostream &operator<<(std::ostream &stream, Token &token) {
        // TODO: Include line number as well.
        return stream << "Token("
            << token.value
            << ", "
            << token.kind
            << ", "
            << token.startPosition
            << "-"
            << token.getEndPosition()
            << ")";
    }
}
