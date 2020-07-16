#pragma once

#include <ionshared/lexical/bare_token.h>
#include "token_kind.h"

namespace ionlang {
    class Token : public ionshared::BareToken<TokenKind> {
    public:
        Token(TokenKind kind, std::string value, uint32_t startPosition = 0, uint32_t lineNumber = 0);
    };

    std::ostream &operator<<(std::ostream &stream, Token &token);

    /**
     * An iterable list of IonLang tokens.
     */
    typedef ionshared::Iterable<Token> TokenStream;
}
