#include <algorithm>
#include <utility>
#include <ionlang/misc/util.h>
#include <ionlang/const/const_name.h>
#include <ionlang/const/grammar.h>

namespace ionlang {
    bool Grammar::isInitialized = false;

    ionshared::BiMap<std::string, TokenKind> Grammar::simple =
        ionshared::BiMap<std::string, TokenKind>();

    const std::vector<std::pair<std::regex, TokenKind>> Grammar::complex({
        {const_regex::string, TokenKind::LiteralString},
        {const_regex::decimal, TokenKind::LiteralDecimal},
        {const_regex::integer, TokenKind::LiteralInteger},
        {const_regex::boolean, TokenKind::LiteralBoolean},
        {const_regex::character, TokenKind::LiteralCharacter},
        {const_regex::whitespace, TokenKind::Whitespace},

        /**
         * NOTE: Identifier regex MUST be placed last, otherwise it will gain
         * precedence over other regexes, for example booleans.
         */
        {const_regex::identifier, TokenKind::Identifier}
    });

    const ionshared::BiMap<std::string, TokenKind> Grammar::symbols({
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
    });

    const ionshared::BiMap<std::string, TokenKind> Grammar::intrinsicOperators({
        {"+", TokenKind::OperatorAddition},
        {"-", TokenKind::OperatorSubtraction},
        {"*", TokenKind::OperatorMultiplication},
        {"/", TokenKind::OperatorDivision},
        {"%", TokenKind::OperatorModulo},
        {"^", TokenKind::OperatorExponent},
        {">", TokenKind::OperatorGreaterThan},
        {"<", TokenKind::OperatorLessThan}
    });

    const std::map<IntrinsicOperatorKind, uint32_t> Grammar::intrinsicOperatorPrecedences({
        {IntrinsicOperatorKind::GreaterThan, 10},
        {IntrinsicOperatorKind::Addition, 20},
        {IntrinsicOperatorKind::Subtraction, 20},
        {IntrinsicOperatorKind::Multiplication, 40},
        {IntrinsicOperatorKind::Division, 40},
        {IntrinsicOperatorKind::Modulo, 40},
        {IntrinsicOperatorKind::Exponent, 60}
    });

    TokenKindVector Grammar::types = {
        TokenKind::TypeVoid,
        TokenKind::TypeBool,
        TokenKind::TypeInt8,
        TokenKind::TypeInt16,
        TokenKind::TypeInt32,
        TokenKind::TypeInt64,
        // TODO
//        TokenKind::TypeUnsignedInt16,
//        TokenKind::TypeUnsignedInt32,
//        TokenKind::TypeUnsignedInt64,
        TokenKind::TypeFloat16,
        TokenKind::TypeFloat32,
        TokenKind::TypeFloat64,
        TokenKind::TypeChar,
        TokenKind::TypeString
    };

    bool Grammar::pushSimple(std::string value, TokenKind tokenKind) {
        return Grammar::simple.insert(std::move(value), tokenKind);
    }

    bool Grammar::sortByKeyLength(
        const std::pair<std::string, TokenKind>& a,
        const std::pair<std::string, TokenKind>& b
    ) {
        return a.first > b.first;
    }

    void Grammar::ensureInit() {
        if (!Grammar::getIsInitialized()) {
            throw std::runtime_error("Constants not initialized");
        }
    }

    bool Grammar::contains(std::vector<TokenKind> subject, TokenKind item) {
        return std::find(subject.begin(), subject.end(), item) != subject.end();
    }

    const ionshared::BiMap<std::string, TokenKind> &Grammar::getSimpleIds() {
        return Grammar::simple;
    }

    const SimplePairVector Grammar::getSortedSimpleIds() {
        Grammar::ensureInit();

        SimplePairVector result = {};

        for (const auto &pair : Grammar::simple.firstMap.unwrapConst()) {
            result.push_back(pair);
        }

        std::sort(result.begin(), result.end(), Grammar::sortByKeyLength);

        return result;
    }

    const TokenKindVector& Grammar::getBuiltInTypes() {
        Grammar::ensureInit();

        return Grammar::types;
    }

    std::map<TokenKind, std::string> Grammar::getNames() {
        Grammar::ensureInit();

        return Grammar::names;
    }

    std::optional<std::string> Grammar::getTokenKindName(TokenKind tokenKind) {
        Grammar::ensureInit();

        if (!ionshared::util::mapContains<TokenKind, std::string>(Grammar::names, tokenKind)) {
            return std::nullopt;
        }

        return Grammar::names[tokenKind];
    }

    bool Grammar::getIsInitialized() {
        return Grammar::isInitialized;
    }

    std::optional<std::string> Grammar::findSimpleValue(TokenKind tokenKind) {
        for (const auto &entry : Grammar::simple.firstMap.unwrapConst()) {
            if (entry.second == tokenKind) {
                return entry.first;
            }
        }

        return std::nullopt;
    }

    void Grammar::init() {
        // Static members have already been initialized. Do not continue.
        if (Grammar::isInitialized) {
            return;
        }

        // Initialize keywords bidirectional map.
        Grammar::keywords = ionshared::BiMap<std::string, TokenKind>(std::map<std::string, TokenKind>{
            // Keywords.
            {"fn", TokenKind::KeywordFunction},
            {"module", TokenKind::KeywordModule},
            {"extern", TokenKind::KeywordExtern},
            {"global", TokenKind::KeywordGlobal},
            {"else", TokenKind::KeywordElse},
            {"unsafe", TokenKind::KeywordUnsafe},
            {"struct", TokenKind::KeywordStruct},

            // Statement keywords.
            {const_name::statementReturn, TokenKind::KeywordReturn},
            {const_name::statementIf, TokenKind::KeywordIf},

            // Type keywords.
            {const_name::typeVoid, TokenKind::TypeVoid},
            {const_name::typeBool, TokenKind::TypeBool},
            {const_name::typeInt8, TokenKind::TypeInt8},
            {const_name::typeInt16, TokenKind::TypeInt16},
            {const_name::typeInt32, TokenKind::TypeInt32},
            {const_name::typeInt64, TokenKind::TypeInt64},
            // TODO: Int128?
            // TODO
//            {const_name::typeUnsignedInt16, TokenKind::TypeUnsignedInt16},
//            {const_name::typeUnsignedInt32, TokenKind::TypeUnsignedInt32},
//            {const_name::typeUnsignedInt64, TokenKind::TypeUnsignedInt64},
            {const_name::typeFloat16, TokenKind::TypeFloat16},
            {const_name::typeFloat32, TokenKind::TypeFloat32},
            {const_name::typeFloat64, TokenKind::TypeFloat64},
            {const_name::typeChar, TokenKind::TypeChar},
            {const_name::typeString, TokenKind::TypeString},

            // Qualifier keywords.
            {"const", TokenKind::QualifierConst},
            {"mut", TokenKind::QualifierMutable}
        });

        // TODO: What is this syntax? Investigate -- might be wrong. Are we using the wrong initialization syntax?
        // Merge simple maps.
        Grammar::simple = Grammar::symbols.merge(Grammar::simple);
        Grammar::simple = Grammar::keywords.merge(Grammar::simple);
        Grammar::simple = Grammar::intrinsicOperators.merge(Grammar::simple);

        /**
         * Raise initialized flag to prevent further
         * attempts to re-initialize.
         */
        Grammar::isInitialized = true;
    }
}
