#include <ionlang/lexical/token_kind.h>

namespace ionlang {
    std::ostream &operator<<(std::ostream &stream, const TokenKind &tokenKind) {
        return stream << "TokenKind(" << (int)tokenKind << ")";
    }
}
