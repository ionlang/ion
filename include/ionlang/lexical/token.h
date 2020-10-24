#pragma once

#include <ionshared/lexical/base_token.h>
#include "token_kind.h"

namespace ionlang {
    typedef ionshared::BaseToken<TokenKind> Token;

    std::ostream& operator<<(std::ostream& stream, Token& token);

    /**
     * An iterable list of IonLang tokens.
     */
    typedef ionshared::Iterable<Token> TokenStream;
}
