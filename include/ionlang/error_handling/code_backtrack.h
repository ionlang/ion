#pragma once

#define IONLANG_CODE_BACKTRACK_DEFAULT_GRACE 2

#include <string>
#include <vector>
#include <ionshared/error_handling/notice.h>
#include <ionlang/lexical/token.h>

namespace ionlang {
    struct CodeBlockLine {
        std::string text;

        std::vector<Token> tokens;

        std::optional<uint32_t> lineNumber = std::nullopt;

        bool highlight = false;
    };

    typedef std::vector<CodeBlockLine> CodeBlock;

    class CodeBacktrack {
    private:
        std::string input;

        TokenStream stream;

    protected:
        static std::string resolveInputText(
            const std::string &input,
            std::vector<Token> lineBuffer
        );

    public:
        CodeBacktrack(std::string input, TokenStream stream);

        [[nodiscard]] std::string getInput() const;

        [[nodiscard]] TokenStream getTokenStream() const;

        std::optional<CodeBlock> createCodeBlockNear(
            uint32_t lineNumber,
            uint32_t grace = IONLANG_CODE_BACKTRACK_DEFAULT_GRACE
        );

        std::optional<CodeBlock> createCodeBlockNear(
            const Token &token,
            uint32_t grace = IONLANG_CODE_BACKTRACK_DEFAULT_GRACE
        );

        std::optional<CodeBlock> createCodeBlockNear(
            const ionshared::SourceLocation &sourceLocation,
            uint32_t grace = IONLANG_CODE_BACKTRACK_DEFAULT_GRACE
        );

        std::optional<CodeBlock> createCodeBlockNear(
            const ionshared::Diagnostic &diagnostic,
            uint32_t grace = IONLANG_CODE_BACKTRACK_DEFAULT_GRACE
        );
    };
}
