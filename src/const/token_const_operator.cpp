#include <ionlang/const/token_const.h>

namespace ionlang {
    ionshared::BiMap<std::string, TokenKind> TokenConst::operators = ionshared::BiMap<std::string, TokenKind>(std::map<std::string, TokenKind>{
        {"+", TokenKind::OperatorAddition},
        {"-", TokenKind::OperatorSubtraction},
        {"*", TokenKind::OperatorMultiplication},
        {"/", TokenKind::OperatorDivision},
        {"%", TokenKind::OperatorModulo},
        {"^", TokenKind::OperatorExponent},
        {">", TokenKind::OperatorGreaterThan},
        {"<", TokenKind::OperatorLessThan}
    });
}
