#pragma once

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <stdexcept>
#include <optional>
#include <utility>
#include <ionshared/misc/iterable.h>
#include <ionlang/const/token_const.h>
#include <ionlang/misc/util.h>
#include <ionlang/misc/regex.h>
#include "token.h"

namespace ionlang {
    struct MatchResult {
        bool success;

        std::optional<std::string> matchedValue = std::nullopt;

        std::optional<std::string> capturedValue = std::nullopt;
    };

    struct MatchOpts {
        Token &token;

        TokenKind tokenKind;

        std::regex regex;

        bool expectCapturedValue = false;
    };

    class Lexer : public ionshared::Generator<Token> {
    private:
        std::string input;

        size_t length;

        size_t index;

        SimplePairVector simpleIds;

        std::vector<std::pair<std::regex, TokenKind>> complexIds;

    protected:
        char getChar() const noexcept;

        std::string getCharAsString() const noexcept;

        size_t getLength() const noexcept;

        size_t setIndex(size_t index) noexcept;

        size_t skip(size_t amount = 1);

        MatchResult matchExpression(MatchOpts opts);

        void processWhitespace();

    public:
        explicit Lexer(const std::string &input);

        size_t getIndex() const noexcept;

        void begin() override;

        /**
         * Whether the index has not reached the input's length.
         */
        bool hasNext() const override;

        /**
         * Process the next token.
         */
        std::optional<Token> tryNext() override;

        std::string getInput() const noexcept;

        std::vector<Token> scan();
    };
}
