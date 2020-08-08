#include <ionlang/syntax/parser.h>

namespace ionlang {
    bool Parser::is(TokenKind tokenKind) noexcept {
        return this->stream.get().getKind() == tokenKind;
    }

    bool Parser::isNext(TokenKind tokenKind) {
        return this->stream.peek()->getKind() == tokenKind;
    }

    Parser::Parser(TokenStream stream, ionshared::StackTrace stackTrace, std::string filePath)
        : stream(stream), stackTrace(stackTrace), filePath(filePath) {
        //
    }

    ionshared::StackTrace Parser::getStackTrace() const {
        return this->stackTrace;
    }

    std::string Parser::getFilePath() const {
        return this->filePath;
    }
}
