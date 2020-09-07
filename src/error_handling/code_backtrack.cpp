#include <sstream>
#include <ionlang/misc/util.h>
#include <ionlang/error_handling/code_backtrack.h>

namespace ionlang {
    std::string CodeBacktrack::resolveInputText(const std::string &input, std::vector<Token> lineBuffer) {
        if (lineBuffer.empty() || input.empty()) {
            throw std::invalid_argument("Both input and line buffer arguments must contain value(s)");
        }

        return lineBuffer.size() > 1
            ? input.substr(lineBuffer[0].getStartPosition(), lineBuffer[lineBuffer.size() - 1].getEndPosition())
            : lineBuffer[0].getValue();
    }

    CodeBacktrack::CodeBacktrack(std::string input, TokenStream stream)
        : input(std::move(input)), stream(std::move(stream)) {
        //
    }

    std::string CodeBacktrack::getInput() const {
        return this->input;
    }

    TokenStream CodeBacktrack::getTokenStream() const {
        return this->stream;
    }

    std::optional<CodeBlock> CodeBacktrack::createCodeBlockNear(uint32_t lineNumber, uint32_t grace) {
        CodeBlock codeBlock = {};

        // Compute start & end line for the code block.
        uint32_t start = grace >= lineNumber ? 0 : lineNumber - grace;
        uint32_t end = lineNumber + grace;

        this->stream.begin();

        uint32_t lineCounter = 0;
        std::optional<Token> tokenBuffer = std::nullopt;
        std::vector<Token> lineBuffer = {};
        bool prime = true;
        bool met = false;

        while (lineCounter != start) {
            if (!this->stream.hasNext()) {
                // Could not reach starting point.
                return std::nullopt;
            }

            tokenBuffer = this->stream.next();

            if (tokenBuffer->getLineNumber() != lineCounter) {
                lineCounter = tokenBuffer->getLineNumber();
            }
        }

        while (lineCounter != end) {
            met = lineCounter >= lineNumber;

            if (!prime) {
                tokenBuffer = this->stream.next();
            }
            else {
                tokenBuffer = this->stream.get();
                prime = false;
            }

            bool streamHasNext = this->stream.hasNext();

            lineBuffer.push_back(*tokenBuffer);

            if (stream.peek()->getLineNumber() != lineCounter) {
                codeBlock.push_back(CodeBlockLine{
                    CodeBacktrack::resolveInputText(input, lineBuffer),
                    lineBuffer,
                    lineNumber
                });

                lineBuffer.clear();
                lineCounter = stream.peek()->getLineNumber();

                continue;
            }

            // Could not reach end point.
            if (!streamHasNext && !met) {
                return std::nullopt;
            }
                // Return requirements have been met. Do not continue.
            else if (!streamHasNext) {
                codeBlock.push_back(CodeBlockLine{
                    CodeBacktrack::resolveInputText(input, lineBuffer),
                    lineBuffer,
                    lineNumber
                });

                return codeBlock;
            }
        }

        return codeBlock;
    }

    std::optional<CodeBlock> CodeBacktrack::createCodeBlockNear(const Token &token, uint32_t grace) {
        return this->createCodeBlockNear(token.getLineNumber(), grace);
    }

    std::optional<CodeBlock> CodeBacktrack::createCodeBlockNear(const ionshared::SourceLocation &sourceLocation, uint32_t grace) {
        return this->createCodeBlockNear(sourceLocation.getLineNumber(), grace);
    }

    std::optional<CodeBlock> CodeBacktrack::createCodeBlockNear(const ionshared::Notice &notice, uint32_t grace) {
        std::optional<ionshared::SourceLocation> location = notice.getLocation();

        if (!location.has_value()) {
            return std::nullopt;
        }
        return this->createCodeBlockNear(*location, grace);
    }
}
