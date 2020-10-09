#include <ionlang/const/grammar.h>

namespace ionlang {
    std::ostream &operator<<(std::ostream &stream, const TokenKind &tokenKind) {
        return stream << Grammar::getTokenKindName(tokenKind).value_or("Unknown ("
            + std::to_string((int)tokenKind)
            + ")"
        );
    }
}
