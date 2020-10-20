#pragma once

#include <optional>
#include <utility>
#include <vector>
#include <map>
#include <string>
#include <regex>
#include <ionshared/container/bi_map.h>
#include <ionlang/construct/expression/operation.h>
#include <ionlang/lexical/token_kind.h>
#include <ionlang/misc/regex.h>
#include <ionlang/misc/helpers.h>
#include <ionlang/const/const_name.h>

namespace ionlang {
    typedef std::vector<std::pair<std::string, TokenKind>> SimplePairVector;

    class Grammar {
    private:
        static bool isInitialized;

        static SimplePairVector simpleRules;

        static TokenKindVector types;

        static bool sortByKeyLength(
            const std::pair<std::string, TokenKind>& a,
            const std::pair<std::string, TokenKind>& b
        );

        static void ensureInit();

    public:
        static const std::map<TokenKind, std::string> tokenKindNames;

        static const std::vector<std::pair<std::regex, TokenKind>> complexRules;

        static const ionshared::BiMap<std::string, TokenKind> keywords;

        static const ionshared::BiMap<std::string, TokenKind> symbols;

        static const ionshared::BiMap<std::string, TokenKind> intrinsicOperators;

        static const std::map<IntrinsicOperatorKind, uint32_t> intrinsicOperatorPrecedences;

        [[nodiscard]] static bool contains(TokenKindVector subject, TokenKind item);

        [[nodiscard]] static const SimplePairVector& getSimpleRules();

        [[nodiscard]] static const TokenKindVector& getBuiltInTypes();

        [[nodiscard]] static bool getIsInitialized();

        [[nodiscard]] static std::string findTokenKindNameOr(
            TokenKind tokenKind,
            std::string alternative = const_name::unknown
        );

        static void init();
    };
}
