#include <ionlang/const/token_const.h>

namespace ionlang {
    ionshared::BiMap<std::string, TokenKind> TokenConst::operators = ionshared::BiMap<std::string, TokenKind>(std::map<std::string, TokenKind>{
        {"+", TokenKind::OperatorAdd},
        {"-", TokenKind::OperatorSubtract},
        {"*", TokenKind::OperatorMultiply},
        {"/", TokenKind::OperatorDivide},
        {"%", TokenKind::OperatorModulo},
        {"^", TokenKind::OperatorExponent},
        {">", TokenKind::OperatorGreaterThan},
        {"<", TokenKind::OperatorLessThan}
    });
}
