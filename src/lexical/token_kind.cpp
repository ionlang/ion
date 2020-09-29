#include <ionlang/const/token_const.h>

namespace ionlang {
    std::ostream &operator<<(std::ostream &stream, const TokenKind &tokenKind) {
        return stream << TokenConst::getTokenKindName(tokenKind).value_or("Unknown ("
            + std::to_string((int)tokenKind)
            + ")"
        );
    }
}
