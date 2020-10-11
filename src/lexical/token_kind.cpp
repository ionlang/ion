#include <ionlang/const/grammar.h>

namespace ionlang {
    std::ostream &operator<<(std::ostream &stream, const TokenKind &tokenKind) {
        std::string name = "Unknown ("
            + std::to_string((int)tokenKind)
            + ")";

        if (Grammar::tokenKindNames.contains(tokenKind)) {
            name = Grammar::tokenKindNames.at(tokenKind);
        }

        return stream << name;
    }
}
