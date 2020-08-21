#include <ionlang/syntax/parser.h>

namespace ionlang {
    bool Parser::is(TokenKind tokenKind) noexcept {
        return this->stream.get().getKind() == tokenKind;
    }

    bool Parser::isNext(TokenKind tokenKind) {
        return this->stream.peek()->getKind() == tokenKind;
    }

    bool Parser::expect(TokenKind tokenKind) {
        if (!this->is(tokenKind)) {
            this->makeNotice("Expected token kind: " + std::to_string((int)tokenKind) + ", but got: " +
                std::to_string((int)this->stream.get().getKind()));

            return false;
        }

        return true;
    }

    bool Parser::skipOver(TokenKind tokenKind) {
        if (!this->expect(tokenKind)) {
            return false;
        }

        this->stream.skip();

        return true;
    }

    ionshared::NoticeFactory Parser::createNoticeFactory() noexcept {
        // Abstract current Token for easier access.
        Token token = this->stream.get();

        return ionshared::NoticeFactory(ionshared::NoticeContext{
            this->filePath,
            token.getLineNumber(),
            token.getStartPosition()
        });
    }

    std::nullopt_t Parser::makeNotice(std::string message, ionshared::NoticeType type) {
        this->noticeStack->push(this->createNoticeFactory().make(type, std::move(message)));

        return std::nullopt;
    }

    Parser::Parser(TokenStream stream, const ionshared::Ptr<ionshared::NoticeStack> &noticeStack, std::string filePath)
        : stream(std::move(stream)), noticeStack(noticeStack), noticeSentinel(std::make_shared<NoticeSentinel>(noticeStack)), filePath(std::move(filePath)) {
        //
    }

    ionshared::Ptr<NoticeSentinel> Parser::getNoticeSentinel() const {
        return this->noticeSentinel;
    }

    std::string Parser::getFilePath() const {
        return this->filePath;
    }

    ionshared::OptPtr<Construct> Parser::parseTopLevel(const ionshared::Ptr<Module> &parent) {
        switch (this->stream.get().getKind()) {
            case TokenKind::KeywordFunction: {
                return this->parseFunction(parent);
            }

            case TokenKind::KeywordGlobal: {
                return this->parseGlobal();
            }

            case TokenKind::KeywordExtern: {
                return this->parseExtern(parent);
            }

            default: {
                return this->makeNotice("Unknown top-level construct");
            }
        }
    }
}
