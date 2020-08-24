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

        std::string getInput() const;

        TokenStream getTokenStream() const;

        std::optional<CodeBlock> createCodeBlockNear(
            uint32_t lineNumber,
            uint32_t grace = IONLANG_CODE_BACKTRACK_DEFAULT_GRACE
        );

        std::optional<CodeBlock> createCodeBlockNear(
            const Token &token,
            uint32_t grace = IONLANG_CODE_BACKTRACK_DEFAULT_GRACE
        );

        std::optional<CodeBlock> createCodeBlockNear(
            const ionshared::NoticeContext &noticeContext,
            uint32_t grace = IONLANG_CODE_BACKTRACK_DEFAULT_GRACE
        );

        std::optional<CodeBlock> createCodeBlockNear(
            const ionshared::Notice &notice,
            uint32_t grace = IONLANG_CODE_BACKTRACK_DEFAULT_GRACE
        );
    };
}
