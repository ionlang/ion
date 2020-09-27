#include <ionlang/const/token_const.h>

namespace ionlang {
    ionshared::BiMap<std::string, TokenKind> TokenConst::symbols = ionshared::BiMap<std::string, TokenKind>(std::map<std::string, TokenKind>({
        {"$", TokenKind::SymbolDollar},
        {"#", TokenKind::SymbolHash},
        {"(", TokenKind::SymbolParenthesesL},
        {")", TokenKind::SymbolParenthesesR},
        {"[", TokenKind::SymbolBracketL},
        {"]", TokenKind::SymbolBracketR},
        {",", TokenKind::SymbolComma},
        {"=", TokenKind::SymbolEqual},
        {";", TokenKind::SymbolSemiColon},
        {"{", TokenKind::SymbolBraceL},
        {"}", TokenKind::SymbolBraceR},
        {"->", TokenKind::SymbolArrow},
        {"&", TokenKind::SymbolAmpersand},
        {"@", TokenKind::SymbolAt},
        {"...", TokenKind::SymbolEllipsis}
    }));
}
