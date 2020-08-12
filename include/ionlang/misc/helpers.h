#pragma once

#include <map>
#include <string>
#include <vector>
#include <ionlang/lexical/token_kind.h>

namespace ionlang {
    typedef std::vector<TokenKind> TokenKindVector;

    typedef std::map<std::string, TokenKind> TokenKindMap;
}
